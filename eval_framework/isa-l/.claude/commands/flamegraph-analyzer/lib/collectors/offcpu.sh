#!/usr/bin/env bash
# lib/collectors/offcpu.sh - Off-CPU blocking analysis collector

log_info "Starting Off-CPU profiling for ${DURATION}s..."

PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_offcpu"

collect_offcpu_perf() {
    local raw="${OUTPUT_DIR}/${PREFIX}_offcpu_raw.perf"
    local -a perf_args=(record -e sched:sched_switch -g -o "${raw}")

    if [[ -n "$CPUS" ]]; then
        perf_args+=(-C "${CPUS}")
    fi

    if [[ -n "$PID" ]]; then
        perf_args+=(-p "${PID}" -- sleep "${DURATION}")
    elif [[ -n "$COMMAND" ]]; then
        perf_args+=(-- ${COMMAND})
    else
        perf_args+=(-a -- sleep "${DURATION}")
    fi

    log_info "Running: perf ${perf_args[*]}"
    perf "${perf_args[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_offcpu_perf.log" || true

    [[ -f "$raw" ]] || die "perf off-cpu record failed: no output file"

    local perf_script="${PREFIX_OUT}_script.txt"
    perf script -i "$raw" > "$perf_script"

    local meta="Type: Off-CPU | Duration: ${DURATION}s | Tool: perf"
    [[ -n "$PID" ]] && meta="${meta} | PID: ${PID}"
    [[ -n "$CPUS" ]] && meta="${meta} | CPU Cores: ${CPUS}"

    full_pipeline "$perf_script" "stackcollapse-perf.pl" "$PREFIX_OUT" "$meta" "--reverse"

    rm -f "$raw" "$perf_script"
}

collect_offcpu_bpftrace() {
    local bt_script
    if [[ -n "$PID" ]]; then
        bt_script="
kprobe:finish_task_switch
/pid == ${PID}/
{
    @start[tid] = nsecs;
}
kretprobe:try_to_wake_up
/pid == ${PID}/
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
    elif [[ -n "$CPUS" ]]; then
        local cpu_filter=""
        IFS=',' read -ra cpu_parts <<< "$CPUS"
        for part in "${cpu_parts[@]}"; do
            if [[ "$part" == *-* ]]; then
                IFS='-' read -r start end <<< "$part"
                for ((i=start; i<=end; i++)); do
                    [[ -n "$cpu_filter" ]] && cpu_filter="${cpu_filter} || "
                    cpu_filter="${cpu_filter}cpu == ${i}"
                done
            else
                [[ -n "$cpu_filter" ]] && cpu_filter="${cpu_filter} || "
                cpu_filter="${cpu_filter}cpu == ${part}"
            fi
        done
        bt_script="
kprobe:finish_task_switch
/${cpu_filter}/
{
    @start[tid] = nsecs;
}
kretprobe:try_to_wake_up
/${cpu_filter}/
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
    else
        bt_script="
kprobe:finish_task_switch
{
    @start[tid] = nsecs;
}
kretprobe:try_to_wake_up
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
    fi

    local raw="${PREFIX_OUT}_raw.txt"
    bpftrace -e "$bt_script" -o "$raw" 2>&1 || true

    if [[ -f "$raw" && -s "$raw" ]]; then
        local folded="${PREFIX_OUT}.folded"
        # Convert bpftrace output to folded format
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
        ' "$raw" > "$folded" 2>/dev/null || true

        if [[ -s "$folded" ]]; then
            local svg="${PREFIX_OUT}.svg"
            local meta="Type: Off-CPU | Duration: ${DURATION}s | Tool: bpftrace"
            [[ -n "$PID" ]] && meta="${meta} | PID: ${PID}"
            [[ -n "$CPUS" ]] && meta="${meta} | CPU Cores: ${CPUS}"

            render_svg "$folded" "$svg" "$meta" "--reverse"
            generate_report "$folded" "${PREFIX_OUT}.report.txt" "$meta"
            rm -f "$raw"
        else
            log_warn "bpftrace off-CPU collection produced no data, trying perf fallback..."
            collect_offcpu_perf
        fi
    else
        log_warn "bpftrace collection failed, trying perf fallback..."
        collect_offcpu_perf
    fi
}

# Dispatch: try bpftrace first, fall back to perf
if command -v bpftrace &>/dev/null; then
    log_info "Using bpftrace for off-CPU tracing"
    collect_offcpu_bpftrace
else
    log_warn "bpftrace not found, falling back to perf sched events"
    collect_offcpu_perf
fi

log_info "Off-CPU flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: ${PREFIX_OUT}.folded and ${PREFIX_OUT}.report.txt"
