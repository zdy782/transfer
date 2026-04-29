#!/usr/bin/env bash
# lib/common.sh - Shared functions for flamegraph toolkit

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

log_info()  { echo -e "${GREEN}[INFO]${NC} $*"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
log_error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }

die() {
    log_error "$@"
    exit 1
}

# Check if a command exists
require_cmd() {
    command -v "$1" &>/dev/null || die "Required command not found: $1. Install it or run: $0 --setup"
}

# Resolve FlameGraph directory (searches common locations)
find_flamegraph_dir() {
    local script_dir
    script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    local candidates=(
        "${script_dir}/../FlameGraph"
        "${script_dir}/../../FlameGraph"
        "$(pwd)/FlameGraph"
    )
    for dir in "${candidates[@]}"; do
        if [[ -f "${dir}/flamegraph.pl" ]]; then
            echo "$(cd "$dir" && pwd)"
            return 0
        fi
    done
    return 1
}

# Ensure output directory exists
ensure_output_dir() {
    local outdir="$1"
    mkdir -p "$outdir"
    echo "$outdir"
}

# Generate timestamp-based filename prefix
output_prefix() {
    date +"%Y%m%d_%H%M%S"
}

# Validate CPU list format (e.g., 0, 0-3, 0,2,4)
validate_cpu_list() {
    local cpus="$1"
    [[ "$cpus" =~ ^[0-9,-]+$ ]] || die "Invalid CPU list format: $cpus (expected: 0, 0-3, 0,2,4)"
}

# Validate PID is a running process
validate_pid() {
    local pid="$1"
    [[ "$pid" =~ ^[0-9]+$ ]] || die "Invalid PID: $pid"
    kill -0 "$pid" 2>/dev/null || die "Process $pid not found or no permission"
}

# Check whether raw perf data should be kept for annotation
should_cleanup_raw() {
    [[ "${ANNOTATE:-false}" != "true" ]]
}
