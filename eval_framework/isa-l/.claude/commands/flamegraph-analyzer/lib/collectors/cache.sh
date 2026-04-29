#!/usr/bin/env bash
# lib/collectors/cache.sh - Cache and branch miss analysis collector

log_info "Starting cache/branch miss analysis for ${DURATION}s..."

PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_cache"
RAW="${OUTPUT_DIR}/${PREFIX}_cache_raw.perf"

# Determine which events to profile
CACHE_TYPE_RESOLVED="${CACHE_TYPE:-all}"

case "$CACHE_TYPE_RESOLVED" in
    cache-misses)            PERF_EVENTS="cache-misses" ;;
    branch-misses)           PERF_EVENTS="branch-misses" ;;
    L1-dcache-load-misses)   PERF_EVENTS="L1-dcache-load-misses" ;;
    dTLB-load-misses)        PERF_EVENTS="dTLB-load-misses" ;;
    all)                     PERF_EVENTS="cache-misses,branch-misses" ;;
    *) die "Invalid cache type: $CACHE_TYPE_RESOLVED" ;;
esac

# Validate events are available
EVENTS_AVAILABLE=true
IFS=',' read -ra EVENT_LIST <<< "$PERF_EVENTS"
for evt in "${EVENT_LIST[@]}"; do
    if ! perf list 2>/dev/null | grep -q "$evt"; then
        log_warn "Hardware event '$evt' not available"
        EVENTS_AVAILABLE=false
    fi
done

if ! $EVENTS_AVAILABLE; then
    log_warn "Hardware cache events unavailable, falling back to cpu-clock (software event)"
    log_warn "Cache miss flame graph will show CPU hotspots instead of actual cache behavior"
    PERF_EVENTS="cpu-clock"
fi

# Build perf arguments
PERF_ARGS=(record -e "${PERF_EVENTS}" -F "${FREQ}" -g -o "${RAW}")

if [[ -n "$CPUS" ]]; then
    PERF_ARGS+=(-C "${CPUS}")
fi

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
perf "${PERF_ARGS[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_cache_perf.log" || true

[[ -f "$RAW" ]] || die "perf record failed: no output file"

# Convert to script output
PERF_SCRIPT="${PREFIX_OUT}_script.txt"
perf script -i "$RAW" > "$PERF_SCRIPT"

# Build metadata
META="Type: Cache Miss | Duration: ${DURATION}s | Events: ${PERF_EVENTS}"
[[ -n "$PID" ]] && META="${META} | PID: ${PID}"
[[ -n "$CPUS" ]] && META="${META} | CPU Cores: ${CPUS}"

# Run pipeline with cache-specific options
full_pipeline "$PERF_SCRIPT" "stackcollapse-perf.pl" "$PREFIX_OUT" "$META" \
    "--countname" "misses"

# Set PERF_DATA_FILE for annotate support
if ! should_cleanup_raw; then
    PERF_DATA_FILE="$RAW"
fi

# Cleanup
rm -f "$PERF_SCRIPT"
if should_cleanup_raw; then
    rm -f "$RAW"
fi

log_info "Cache miss flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: ${PREFIX_OUT}.folded and ${PREFIX_OUT}.report.txt"
