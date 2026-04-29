#!/usr/bin/env bash
# lib/collectors/cpu.sh - CPU profiling collector

log_info "Starting CPU profiling for ${DURATION}s..."

RAW="${OUTPUT_DIR}/${PREFIX}_cpu_raw.perf"
PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_cpu"

# Detect if hardware PMU is available; fall back to cpu-clock (software event)
# VMs/containers often have broken hardware counters (max_precise=0, no samples)
PERF_EVENT="cycles"
if perf record -e cycles -F 99 -o /dev/null -- sleep 0.01 2>/dev/null; then
    # Verify it actually captured samples
    if perf script -i /dev/null 2>/dev/null | head -1 | grep -q '.' 2>/dev/null; then
        PERF_EVENT="cycles"
    else
        log_warn "Hardware PMU unavailable, using cpu-clock (software event)"
        PERF_EVENT="cpu-clock"
    fi
else
    log_warn "Hardware PMU unavailable, using cpu-clock (software event)"
    PERF_EVENT="cpu-clock"
fi

# Build perf record arguments
PERF_ARGS=(record -e "${PERF_EVENT}" -F "${FREQ}" -g -o "${RAW}")

# Add CPU filter
if [[ -n "$CPUS" ]]; then
    PERF_ARGS+=(-C "${CPUS}")
fi

# Add target
if [[ -n "$PID" ]]; then
    PERF_ARGS+=(-p "${PID}" -- sleep "${DURATION}")
elif [[ -n "$COMMAND" ]]; then
    PERF_ARGS+=(-- ${COMMAND})
elif [[ -n "$CPUS" ]]; then
    PERF_ARGS+=(-a -- sleep "${DURATION}")
else
    die "No target specified. Use -p, -c, or -C."
fi

# Collect
log_info "Running: perf ${PERF_ARGS[*]}"
perf "${PERF_ARGS[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_cpu_perf.log" || true

[[ -f "$RAW" ]] || die "perf record failed: no output file"

# Convert to script output
PERF_SCRIPT="${PREFIX_OUT}_script.txt"
perf script -i "$RAW" > "$PERF_SCRIPT"

# Build metadata
META="Type: CPU | Duration: ${DURATION}s | Freq: ${FREQ}Hz"
[[ -n "$PID" ]] && META="${META} | PID: ${PID}"
[[ -n "$CPUS" ]] && META="${META} | CPU Cores: ${CPUS}"

# Run full pipeline: collapse, render SVG, generate report
FOLDED="$(full_pipeline "$PERF_SCRIPT" "stackcollapse-perf.pl" "$PREFIX_OUT" "$META")"

# Set PERF_DATA_FILE for annotate support
if ! should_cleanup_raw; then
    PERF_DATA_FILE="$RAW"
fi

# Cleanup raw files (skip perf.data if annotate mode)
rm -f "$PERF_SCRIPT"
if should_cleanup_raw; then
    rm -f "$RAW"
fi

log_info "CPU flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: ${FOLDED} and ${PREFIX_OUT}.report.txt"
