#!/usr/bin/env bash
# flamegraph.sh - Flame Graph Generation Toolkit
# Usage: ./flamegraph.sh -t <type> [options]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Source common utilities
source "${SCRIPT_DIR}/lib/common.sh"

# Default values
TYPE=""
PID=""
COMMAND=""
CPUS=""
DURATION=30
FREQ=99
OUTPUT_DIR="${SCRIPT_DIR}/output"
INPUT_FILE=""
SCREENSHOT=false
ICICLE=false
ANNOTATE=false
BASELINE=""
CURRENT=""
CACHE_TYPE=""

usage() {
    cat <<EOF
Flame Graph Toolkit - Generate interactive flame graphs with AI-friendly reports

Usage: $0 -t <type> [options]

Types:
  cpu       CPU profiling (perf record)
  offcpu    Off-CPU blocking analysis (bpftrace/perf sched)
  mem       Memory allocation analysis (perf mem/bpftrace)
  lock      Lock contention analysis (bpftrace/perf futex)
  cache     Cache/branch miss analysis (perf hardware events)
  generic   Import pre-existing folded stack file (-f required)
  diff      Differential comparison of two folded stack files

Options:
  -t TYPE       Profile type (cpu|offcpu|mem|lock|cache|generic|diff)
  -p PID        Attach to running process
  -c COMMAND    Run command and profile it
  -C CPUS       Filter by CPU cores (e.g., 0, 0-3, 0,2,4)
  -d SECONDS    Collection duration (default: 30)
  -F HZ         Sample frequency in Hz (default: 99)
  -o DIR        Output directory (default: ./output)
  -f FILE       Input folded stack file (generic mode only)
  --baseline F  Baseline folded file (diff mode, required)
  --current F   Current folded file (diff mode, required)
  --cache-type T  Cache event type: cache-misses|branch-misses|L1-dcache-load-misses|dTLB-load-misses|all (default: all)
  --icicle      Generate icicle (top-down) graph instead of flame graph
  --annotate    Generate source-level annotation (cpu/cache modes only)
  --screenshot  Convert SVG to PNG via Playwright
  --setup       Download FlameGraph scripts and check dependencies
  -h, --help    Show this help message

Examples:
  $0 -t cpu -c './myapp' -d 60
  $0 -t cpu -p 1234 -C 0,2-4 -d 30 --icicle
  $0 -t offcpu -p 1234 -d 60
  $0 -t mem -c './myapp' -d 30
  $0 -t lock -c './myapp' -d 30
  $0 -t cache -c './myapp' -d 30 --cache-type cache-misses
  $0 -t cpu -c './myapp' -d 30 --annotate
  $0 -t generic -f stacks.folded
  $0 -t diff --baseline baseline.folded --current current.folded
  $0 --setup
EOF
    exit 0
}

do_setup() {
    log_info "Checking dependencies..."

    # Check perf
    if command -v perf &>/dev/null; then
        log_info "perf: $(perf --version 2>&1 || echo 'found')"
    else
        log_warn "perf not found. Install: apt install linux-tools-common linux-tools-$(uname -r)"
    fi

    # Check bpftrace
    if command -v bpftrace &>/dev/null; then
        log_info "bpftrace: $(bpftrace --version 2>&1 || echo 'found')"
    else
        log_warn "bpftrace not found (optional). Install: apt install bpftrace"
    fi

    # Download FlameGraph
    local fg_dir="${SCRIPT_DIR}/FlameGraph"
    if [[ -f "${fg_dir}/flamegraph.pl" ]]; then
        log_info "FlameGraph already exists at ${fg_dir}"
    else
        log_info "Downloading FlameGraph..."
        curl -fsSL https://github.com/brendangregg/FlameGraph/archive/refs/heads/master.tar.gz \
            | tar xz -C "${SCRIPT_DIR}"
        mv "${SCRIPT_DIR}/FlameGraph-master" "${fg_dir}"
        chmod +x "${fg_dir}"/*.pl
        if [[ ! -f "${fg_dir}/flamegraph.pl" ]]; then
            die "Download failed: flamegraph.pl not found in extracted archive"
        fi
        log_info "FlameGraph downloaded to ${fg_dir}"
    fi

    log_info "Setup complete."
}

# Parse arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -t) TYPE="$2"; shift 2 ;;
            -p) PID="$2"; shift 2 ;;
            -c) COMMAND="$2"; shift 2 ;;
            -C) CPUS="$2"; shift 2 ;;
            -d) DURATION="$2"; shift 2 ;;
            -F) FREQ="$2"; shift 2 ;;
            -o) OUTPUT_DIR="$2"; shift 2 ;;
            -f) INPUT_FILE="$2"; shift 2 ;;
            --screenshot) SCREENSHOT=true; shift ;;
            --icicle) ICICLE=true; shift ;;
            --annotate) ANNOTATE=true; shift ;;
            --baseline) BASELINE="$2"; shift 2 ;;
            --current) CURRENT="$2"; shift 2 ;;
            --cache-type) CACHE_TYPE="$2"; shift 2 ;;
            --setup) do_setup; exit 0 ;;
            -h|--help) usage ;;
            *) die "Unknown option: $1" ;;
        esac
    done
}

parse_args "$@"

# Validate numeric arguments
[[ "$DURATION" =~ ^[0-9]+$ ]] || die "Duration must be a positive integer: $DURATION"
[[ "$FREQ" =~ ^[0-9]+$ ]] || die "Frequency must be a positive integer: $FREQ"

# Validate
[[ -z "$TYPE" ]] && die "Missing required option: -t <type>. Use -h for help."
[[ "$TYPE" =~ ^(cpu|offcpu|mem|lock|cache|generic|diff)$ ]] || die "Invalid type: $TYPE (must be cpu|offcpu|mem|lock|cache|generic|diff)"
[[ -n "$PID" && -n "$COMMAND" ]] && die "Cannot use -p and -c together"
[[ "$TYPE" == "generic" && -z "$INPUT_FILE" ]] && die "Generic mode requires -f <folded_file>"

# Diff-specific validation
if [[ "$TYPE" == "diff" ]]; then
    [[ -z "$BASELINE" ]] && die "Diff mode requires --baseline <file>"
    [[ -z "$CURRENT" ]] && die "Diff mode requires --current <file>"
    [[ -f "$BASELINE" ]] || die "Baseline file not found: $BASELINE"
    [[ -f "$CURRENT" ]] || die "Current file not found: $CURRENT"
    [[ -s "$BASELINE" ]] || die "Baseline file is empty: $BASELINE"
    [[ -s "$CURRENT" ]] || die "Current file is empty: $CURRENT"
fi

# Annotate validation
if $ANNOTATE; then
    [[ "$TYPE" =~ ^(cpu|cache)$ ]] || die "--annotate is only supported with cpu or cache types"
fi

# Cache-type validation
if [[ -n "$CACHE_TYPE" ]]; then
    [[ "$TYPE" == "cache" ]] || die "--cache-type is only valid with -t cache"
    [[ "$CACHE_TYPE" =~ ^(cache-misses|branch-misses|L1-dcache-load-misses|dTLB-load-misses|all)$ ]] \
        || die "Invalid cache type: $CACHE_TYPE (must be cache-misses|branch-misses|L1-dcache-load-misses|dTLB-load-misses|all)"
fi

if [[ -n "$CPUS" ]]; then
    validate_cpu_list "$CPUS"
fi

if [[ -n "$PID" ]]; then
    validate_pid "$PID"
fi

# Ensure at least one target is specified (except generic)
if [[ "$TYPE" != "generic" && "$TYPE" != "diff" ]]; then
    [[ -z "$PID" && -z "$COMMAND" && -z "$CPUS" ]] && die "Specify at least one of: -p <pid>, -c <command>, or -C <cpus>"
fi

# Resolve FlameGraph directory
FG_DIR="$(find_flamegraph_dir)" || die "FlameGraph not found. Run: $0 --setup"

# Prepare output
OUTPUT_DIR="$(ensure_output_dir "$OUTPUT_DIR")"
PREFIX="$(output_prefix)"

log_info "Type: $TYPE | Duration: ${DURATION}s | Output: ${OUTPUT_DIR}"

# Dispatch to collector
COLLECTOR="${SCRIPT_DIR}/lib/collectors/${TYPE}.sh"
[[ -f "$COLLECTOR" ]] || die "Collector not found: ${COLLECTOR}"

source "${SCRIPT_DIR}/lib/render.sh"
source "$COLLECTOR"

# Source annotation if requested
if $ANNOTATE; then
    if [[ -n "${PERF_DATA_FILE:-}" && -f "$PERF_DATA_FILE" ]]; then
        source "${SCRIPT_DIR}/lib/annotate.sh"
        generate_annotation "$PERF_DATA_FILE" "$OUTPUT_DIR" "$PREFIX"
    else
        log_warn "No perf.data available for annotation"
    fi
fi

# Screenshot if requested
if $SCREENSHOT; then
    SVG_FILE=$(ls -t "${OUTPUT_DIR}/${PREFIX}"*.svg 2>/dev/null | head -1)
    if [[ -n "$SVG_FILE" ]]; then
        source "${SCRIPT_DIR}/lib/screenshot.sh"
        svg_to_png "$SVG_FILE" "${SVG_FILE%.svg}.png"
    fi
fi

log_info "Done. Output files in: ${OUTPUT_DIR}"
