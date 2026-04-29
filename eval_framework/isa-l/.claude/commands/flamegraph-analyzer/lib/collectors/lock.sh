#!/usr/bin/env bash
# lib/collectors/lock.sh - Lock contention analysis collector

log_info "Starting lock contention analysis for ${DURATION}s..."

PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_lock"
RAW="${OUTPUT_DIR}/${PREFIX}_lock_raw.perf"

process_perf_data() {
    local perf_script="${PREFIX_OUT}_script.txt"
    perf script -i "$RAW" > "$perf_script"

    local meta="Type: Lock Contention | Duration: ${DURATION}s"
    [[ -n "$PID" ]] && meta="${meta} | PID: ${PID}"
    [[ -n "$CPUS" ]] && meta="${meta} | CPU Cores: ${CPUS}"

    full_pipeline "$perf_script" "stackcollapse-perf.pl" "$PREFIX_OUT" "$meta" \
        "--reverse" "--color" "io" "--countname" "contention_events"

    if should_cleanup_raw; then
        rm -f "$RAW" "$perf_script"
    else
        rm -f "$perf_script"
        PERF_DATA_FILE="$RAW"
    fi
}

collect_lock_tracepoints() {
    local -a perf_args=(record -e lock:contention_begin -g -o "${RAW}")

    if [[ -n "$CPUS" ]]; then perf_args+=(-C "${CPUS}"); fi

    if [[ -n "$PID" ]]; then
        perf_args+=(-p "${PID}" -- sleep "${DURATION}")
    elif [[ -n "$COMMAND" ]]; then
        perf_args+=(-- ${COMMAND})
    elif [[ -n "$CPUS" ]]; then
        perf_args+=(-a -- sleep "${DURATION}")
    else
        perf_args+=(-a -- sleep "${DURATION}")
    fi

    log_info "Running: perf ${perf_args[*]}"
    perf "${perf_args[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_lock_perf.log" || return 1

    [[ -f "$RAW" ]] || return 1
    process_perf_data
}

collect_lock_bpftrace() {
    local pid_filter=""
    if [[ -n "$PID" ]]; then
        pid_filter="/pid == ${PID}/"
    fi

    # Detect available mutex symbols across kernel versions
    local mutex_sym=""
    if grep -q 'T __mutex_lock$' /proc/kallsyms 2>/dev/null; then
        mutex_sym="__mutex_lock"
    elif grep -q 'T mutex_lock_slowpath$' /proc/kallsyms 2>/dev/null; then
        mutex_sym="mutex_lock_slowpath"
    elif grep -q 'T __mutex_lock_slowpath$' /proc/kallsyms 2>/dev/null; then
        mutex_sym="__mutex_lock_slowpath"
    else
        log_warn "No mutex lock symbols found in /proc/kallsyms"
        return 1
    fi

    local bt_script="
kprobe:${mutex_sym} ${pid_filter}
{
    @start[tid] = nsecs;
}
kretprobe:${mutex_sym} ${pid_filter}
{
    if (@start[tid] > 0) {
        @us[@ustack] = sum((nsecs - @start[tid]) / 1000);
    }
    delete(@start[tid]);
}
interval:s:${DURATION} {
    print(@us);
    exit();
}"
    local bt_raw="${PREFIX_OUT}_raw.txt"
    bpftrace -e "$bt_script" -o "$bt_raw" 2>&1 || true

    if [[ -f "$bt_raw" && -s "$bt_raw" ]]; then
        local folded="${PREFIX_OUT}.folded"
        awk '
        /^@/ { next }
        /^[[:space:]]/ {
            gsub(/^[[:space:]]+/, "")
            sub(/:$/, "")
            stack = $0
        }
        /^[0-9]/ {
            printf "%s %s\n", stack, $1
        }
        ' "$bt_raw" > "$folded" 2>/dev/null || true

        if [[ -s "$folded" ]]; then
            local meta="Type: Lock Contention | Duration: ${DURATION}s | Tool: bpftrace"
            [[ -n "$PID" ]] && meta="${meta} | PID: ${PID}"
            [[ -n "$CPUS" ]] && meta="${meta} | CPU Cores: ${CPUS}"

            render_svg "$folded" "${PREFIX_OUT}.svg" "$meta" \
                "--reverse" "--color" "io" "--countname" "contention_us"
            generate_report "$folded" "${PREFIX_OUT}.report.txt" "$meta"
            rm -f "$bt_raw"
            return 0
        else
            log_warn "bpftrace lock collection produced no data"
            rm -f "$bt_raw"
            return 1
        fi
    else
        log_warn "bpftrace lock collection failed"
        rm -f "$bt_raw"
        return 1
    fi
}

collect_lock_futex() {
    local -a perf_args=(record -e syscalls:sys_enter_futex,syscalls:sys_exit_futex -g -o "${RAW}")

    if [[ -n "$CPUS" ]]; then perf_args+=(-C "${CPUS}"); fi

    if [[ -n "$PID" ]]; then
        perf_args+=(-p "${PID}" -- sleep "${DURATION}")
    elif [[ -n "$COMMAND" ]]; then
        perf_args+=(-- ${COMMAND})
    elif [[ -n "$CPUS" ]]; then
        perf_args+=(-a -- sleep "${DURATION}")
    else
        perf_args+=(-a -- sleep "${DURATION}")
    fi

    log_info "Running: perf ${perf_args[*]}"
    perf "${perf_args[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_lock_perf.log" || true

    [[ -f "$RAW" ]] || die "perf futex record failed: no output file"
    process_perf_data
}

# Dispatch: try strategies in order
if perf list 2>/dev/null | grep -q 'lock:contention_begin'; then
    log_info "Using perf lock tracepoints for lock contention"
    collect_lock_tracepoints || {
        log_warn "perf lock tracepoints failed, trying bpftrace..."
        collect_lock_bpftrace || {
            log_warn "bpftrace failed, trying futex syscalls..."
            collect_lock_futex
        }
    }
elif command -v bpftrace &>/dev/null; then
    log_info "Using bpftrace for lock contention"
    collect_lock_bpftrace || {
        log_warn "bpftrace failed, trying futex syscalls..."
        collect_lock_futex
    }
else
    log_warn "Using perf futex syscalls for lock analysis"
    collect_lock_futex
fi

log_info "Lock contention flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: ${PREFIX_OUT}.folded and ${PREFIX_OUT}.report.txt"
