#!/usr/bin/env bash
# lib/render.sh - SVG rendering and AI report generation

# Render flame graph SVG from folded stacks file
# Args: $1 = folded file, $2 = output SVG path, $3 = title, $4+ = extra flamegraph.pl opts
render_svg() {
    local folded="$1"
    local svg="$2"
    local title="$3"
    shift 3
    local -a extra_opts=("$@")

    # Inject icicle options if enabled
    if [[ "${ICICLE:-false}" == "true" ]]; then
        extra_opts=("--reverse" "--inverted" "${extra_opts[@]}")
    fi

    [[ -f "$folded" ]] || die "Folded file not found: $folded"
    [[ -s "$folded" ]] || die "Folded file is empty: $folded"

    local fg_dir
    fg_dir="$(find_flamegraph_dir)"

    perl "${fg_dir}/flamegraph.pl" \
        --title "$title" \
        --width 1200 \
        "${extra_opts[@]}" \
        "$folded" > "$svg"

    log_info "SVG rendered: $svg ($(du -h "$svg" | cut -f1))"
}

# Generate AI-friendly text report from folded stacks
# Args: $1 = folded file, $2 = output report path, $3 = metadata string
generate_report() {
    local folded="$1"
    local report="$2"
    local metadata="$3"

    [[ -f "$folded" ]] || die "Folded file not found: $folded"

    local total_samples
    total_samples=$(awk '{sum += $NF} END {print sum+0}' "$folded")

    {
        echo "=== Flame Graph Report ==="
        echo "$metadata"
        echo "Total Samples: ${total_samples}"
        echo ""
        echo "Top 20 Hot Functions (% total):"
        awk -F';' -v total="$total_samples" '{
            name = $NF
            sub(/ [0-9]+$/, "", name)
            count = $NF
            sub(/.* /, "", count)
            funcs[name] += count
        }
        END {
            n = 0
            for (f in funcs) {
                a[n] = funcs[f] SUBSEP f
                n++
            }
            for (i = 0; i < n; i++) {
                split(a[i], parts, SUBSEP)
                counts[i] = parts[1]
                names[i] = parts[2]
            }
            for (i = 0; i < n-1; i++) {
                for (j = i+1; j < n; j++) {
                    if (counts[j] > counts[i]) {
                        tmp = counts[i]; counts[i] = counts[j]; counts[j] = tmp
                        tmp = names[i]; names[i] = names[j]; names[j] = tmp
                    }
                }
            }
            limit = (n < 20) ? n : 20
            for (i = 0; i < limit; i++) {
                pct = sprintf("%.1f", counts[i] / total * 100)
                printf "  %5s%%  %s\n", pct, names[i]
            }
        }' "$folded"
        echo ""
        echo "Top 10 Call Paths:"
        sort -t' ' -k2 -rn "$folded" | head -10 | while IFS= read -r line; do
            local pct
            pct=$(echo "$line" | awk -v total="${total_samples}" '{printf "%.1f", $NF/total*100}')
            local path
            path=$(echo "$line" | awk '{$NF=""; print $0}' | sed 's/;[[:space:]]*$//' | sed 's/[[:space:]]*$//')
            printf "  %s  [%s%%]\n" "$path" "$pct"
        done
    } > "$report"

    log_info "Report generated: $report"
}

# Full pipeline: collapse stacks, render SVG, generate report
# Args: $1 = raw data file, $2 = collapse script name,
#        $3 = output prefix, $4 = metadata, $5+ = flamegraph.pl extra opts
full_pipeline() {
    local raw="$1"
    local collapse_script="$2"
    local prefix="$3"
    local metadata="$4"
    shift 4
    local fg_extra=("$@")

    local fg_dir
    fg_dir="$(find_flamegraph_dir)"
    local folded="${prefix}.folded"
    local svg="${prefix}.svg"
    local report="${prefix}.report.txt"

    local collapse="${fg_dir}/${collapse_script}"
    [[ -f "$collapse" ]] || die "Collapse script not found: $collapse"

    "$collapse" "$raw" > "$folded"
    log_info "Stacks collapsed: $folded ($(wc -l < "$folded") frames)"

    render_svg "$folded" "$svg" "$metadata" "${fg_extra[@]}"

    generate_report "$folded" "$report" "$metadata"

    echo "$folded"
}

# Generate differential report from difffolded.pl output
# Diff folded format: stack_trace baseline_count current_count
# Args: $1 = diff folded file, $2 = output report path, $3 = metadata string
generate_diff_report() {
    local folded="$1"
    local report="$2"
    local metadata="$3"

    [[ -f "$folded" ]] || die "Diff folded file not found: $folded"

    {
        echo "=== Differential Flame Graph Report ==="
        echo "$metadata"
        echo ""

        # Summary statistics - parse last two space-separated fields from each line
        local total_base total_curr
        total_base=$(awk '{n=split($0,a,";"); split(a[n],b," "); sum+=b[length(b)-1]} END {print sum+0}' "$folded")
        total_curr=$(awk '{n=split($0,a,";"); split(a[n],b," "); sum+=b[length(b)]} END {print sum+0}' "$folded")

        echo "Baseline samples: ${total_base}"
        echo "Current samples:  ${total_curr}"
        if [[ "$total_base" -gt 0 ]]; then
            local change
            change=$(awk "BEGIN {printf \"%.1f\", ($total_curr - $total_base) / $total_base * 100}")
            echo "Total change: ${change}%"
        fi
        echo ""

        # Top 15 regressions (biggest increase)
        echo "Top 15 Regressions (functions with biggest increase):"
        awk -F';' '{
            # Last field: "funcName baseCount currCount"
            last = $NF
            name = last; sub(/ [0-9]+ [0-9]+$/, "", name)
            curr = last; sub(/.* /, "", curr)
            rest = last; sub(/ [0-9]+$/, "", rest)
            base = rest; sub(/.* /, "", base)
            funcs[name] += curr - base
            base_total[name] += base
            curr_total[name] += curr
        }
        END {
            n = 0
            for (f in funcs) {
                delta = funcs[f]
                if (delta > 0) {
                    a[n] = delta SUBSEP f SUBSEP base_total[f] SUBSEP curr_total[f]
                    n++
                }
            }
            for (i = 0; i < n; i++) {
                split(a[i], pi, SUBSEP)
                for (j = i+1; j < n; j++) {
                    split(a[j], pj, SUBSEP)
                    if (pj[1]+0 > pi[1]+0) {
                        tmp = a[i]; a[i] = a[j]; a[j] = tmp
                        split(a[i], pi, SUBSEP)
                    }
                }
            }
            limit = (n < 15) ? n : 15
            for (i = 0; i < limit; i++) {
                split(a[i], parts, SUBSEP)
                printf "  %+6d  %s  (baseline: %d, current: %d)\n", parts[1]+0, parts[2], parts[3]+0, parts[4]+0
            }
        }' "$folded"
        echo ""

        # Top 15 improvements (biggest decrease)
        echo "Top 15 Improvements (functions with biggest decrease):"
        awk -F';' '{
            last = $NF
            name = last; sub(/ [0-9]+ [0-9]+$/, "", name)
            curr = last; sub(/.* /, "", curr)
            rest = last; sub(/ [0-9]+$/, "", rest)
            base = rest; sub(/.* /, "", base)
            funcs[name] += curr - base
            base_total[name] += base
            curr_total[name] += curr
        }
        END {
            n = 0
            for (f in funcs) {
                delta = funcs[f]
                if (delta < 0) {
                    a[n] = delta SUBSEP f SUBSEP base_total[f] SUBSEP curr_total[f]
                    n++
                }
            }
            for (i = 0; i < n; i++) {
                split(a[i], pi, SUBSEP)
                for (j = i+1; j < n; j++) {
                    split(a[j], pj, SUBSEP)
                    if (pj[1]+0 < pi[1]+0) {
                        tmp = a[i]; a[i] = a[j]; a[j] = tmp
                        split(a[i], pi, SUBSEP)
                    }
                }
            }
            limit = (n < 15) ? n : 15
            for (i = 0; i < limit; i++) {
                split(a[i], parts, SUBSEP)
                printf "  %+6d  %s  (baseline: %d, current: %d)\n", parts[1]+0, parts[2], parts[3]+0, parts[4]+0
            }
        }' "$folded"
    } > "$report"

    log_info "Diff report generated: $report"
}
