#!/usr/bin/env bash
# lib/collectors/memory.sh - Memory allocation analysis collector

log_info "Starting memory profiling for ${DURATION}s..."

PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_mem"

collect_mem_perf() {
    local raw="${OUTPUT_DIR}/${PREFIX}_mem_raw.perf"
    local perf_script="${PREFIX_OUT}_script.txt"
    local fg_dir
    fg_dir="$(find_flamegraph_dir)"

    # Build common target args
    local -a target_args=()
    if [[ -n "$CPUS" ]]; then target_args+=(-C "${CPUS}"); fi
    if [[ -n "$PID" ]]; then
        target_args+=(-p "${PID}" -- sleep "${DURATION}")
    elif [[ -n "$COMMAND" ]]; then
        target_args+=(-- ${COMMAND})
    else
        target_args+=(-a -- sleep "${DURATION}")
    fi

    local _mem_ok=false

    # Try perf mem first
    if perf mem record -g -o /dev/null -- sleep 0.01 2>/dev/null; then
        log_info "Running: perf mem record -g -o ${raw} ${target_args[*]}"
        perf mem record -g -o "${raw}" "${target_args[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_mem_perf.log" || true

        if [[ -f "$raw" ]]; then
            perf script -i "$raw" > "$perf_script"
            local folded="${PREFIX_OUT}.folded"
            "${fg_dir}/stackcollapse-perf.pl" "$perf_script" > "$folded"
            local frame_count
            frame_count=$(wc -l < "$folded")
            if [[ "$frame_count" -gt 0 ]]; then
                log_info "Stacks collapsed: $folded ($frame_count frames)"
                _mem_ok=true
            else
                log_warn "perf mem produced no usable stacks (Arm SPE limitation?), trying mmap/brk syscalls"
                rm -f "$raw" "$perf_script" "$folded"
            fi
        fi
    fi

    # Fall back to mmap/brk syscalls
    if [[ "$_mem_ok" != "true" ]]; then
        log_warn "Using syscalls:sys_enter_mmap,sys_enter_brk for memory profiling"
        log_info "Running: perf record -e syscalls:sys_enter_mmap,syscalls:sys_enter_brk -g -o ${raw} ${target_args[*]}"
        perf record -e syscalls:sys_enter_mmap,syscalls:sys_enter_brk -g -o "${raw}" "${target_args[@]}" 2>&1 | tee "${OUTPUT_DIR}/${PREFIX}_mem_perf.log" || true

        [[ -f "$raw" ]] || die "perf mmap/brk record failed: no output file"
        perf script -i "$raw" > "$perf_script"

        local folded="${PREFIX_OUT}.folded"
        "${fg_dir}/stackcollapse-perf.pl" "$perf_script" > "$folded"
        local frame_count
        frame_count=$(wc -l < "$folded")
        log_info "Stacks collapsed: $folded ($frame_count frames)"
    fi

    local meta="Type: Memory | Duration: ${DURATION}s | Tool: perf"
    [[ -n "$PID" ]] && meta="${meta} | PID: ${PID}"
    [[ -n "$CPUS" ]] && meta="${meta} | CPU Cores: ${CPUS}"

    local folded="${PREFIX_OUT}.folded"
    local svg="${PREFIX_OUT}.svg"
    render_svg "$folded" "$svg" "$meta" "--color" "mem"
    generate_report "$folded" "${PREFIX_OUT}.report.txt" "$meta"

    rm -f "$raw" "$perf_script"
}

collect_mem_bpftrace() {
    local bt_filter=""
    if [[ -n "$PID" ]]; then
        bt_filter="/pid == ${PID}/"
    fi

    local libc_path
    # Try ldconfig first (most reliable), then fall back to searching known paths
    libc_path=$(ldconfig -p 2>/dev/null | grep -m1 'libc\.so\.6' | awk '{print $NF}')
    if [[ -z "$libc_path" ]]; then
        # Search known paths, filter to ELF files (skip linker scripts like libc.so)
        while IFS= read -r f; do
            if file "$f" 2>/dev/null | grep -q ELF; then
                libc_path="$f"
                break
            fi
        done < <(find /lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu \
                       /lib/aarch64-linux-gnu /usr/lib/aarch64-linux-gnu \
                       /lib64 /usr/lib64 \
                       -name 'libc.so*' -type f 2>/dev/null)
    fi
    if [[ -z "$libc_path" ]]; then
        log_warn "libc not found, falling back to perf mem"
        collect_mem_perf
        return
    fi

    local bt_script="
uprobe:${libc_path}:malloc ${bt_filter} {
    @[ustack] = sum(arg0);
}
interval:s:${DURATION} {
    print(@);
    exit();
}"
    local raw="${PREFIX_OUT}_raw.txt"
    local bt_err="${PREFIX_OUT}_stderr.txt"
    bpftrace -e "$bt_script" -o "$raw" 2>"$bt_err" || true

    # Check for bpftrace stack resolution failures
    if grep -q 'failed to look up stack id\|ERROR:' "$bt_err" 2>/dev/null; then
        log_warn "bpftrace stack resolution failed, trying perf mem..."
        rm -f "$raw" "$bt_err"
        collect_mem_perf
        return
    fi
    rm -f "$bt_err"

    if [[ -f "$raw" && -s "$raw" ]]; then
        local folded="${PREFIX_OUT}.folded"
        # Parse bpftrace text output: extract stack traces and counts
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
            local meta="Type: Memory | Duration: ${DURATION}s | Tool: bpftrace"
            [[ -n "$PID" ]] && meta="${meta} | PID: ${PID}"
            [[ -n "$CPUS" ]] && meta="${meta} | CPU Cores: ${CPUS}"

            render_svg "$folded" "$svg" "$meta" "--color" "mem"
            generate_report "$folded" "${PREFIX_OUT}.report.txt" "$meta"
        else
            log_warn "bpftrace memory collection produced no data, trying perf mem..."
            collect_mem_perf
        fi
        rm -f "$raw"
    else
        log_warn "bpftrace collection failed, trying perf mem..."
        collect_mem_perf
    fi
}

# Dispatch: try bpftrace first, fall back to perf mem
if command -v bpftrace &>/dev/null; then
    log_info "Using bpftrace for memory tracing"
    collect_mem_bpftrace
else
    log_info "Using perf mem for memory tracing"
    collect_mem_perf
fi

log_info "Memory flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: ${PREFIX_OUT}.folded and ${PREFIX_OUT}.report.txt"
