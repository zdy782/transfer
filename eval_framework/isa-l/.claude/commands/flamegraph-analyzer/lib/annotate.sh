#!/usr/bin/env bash
# lib/annotate.sh - Source-level annotation via perf annotate

generate_annotation() {
    local perf_data="$1"
    local output_dir="$2"
    local prefix="$3"

    local annotate_file="${output_dir}/${prefix}_annotate.txt"

    require_cmd perf

    log_info "Generating source annotation from: $perf_data"

    # Find the corresponding folded file to extract top functions
    local folded_actual=""
    for f in "${output_dir}/${prefix}"_*.folded; do
        [[ -f "$f" ]] && folded_actual="$f" && break
    done

    if [[ -z "$folded_actual" ]]; then
        log_warn "No folded file found for annotation"
        return 1
    fi

    # Extract top 10 function names from the folded file
    local top_funcs
    top_funcs=$(awk -F';' '{
        name = $NF
        sub(/ [0-9]+$/, "", name)
        funcs[name]++
    }
    END {
        n = 0
        for (f in funcs) {
            a[n] = funcs[f] SUBSEP f
            n++
        }
        for (i = 0; i < n-1; i++) {
            split(a[i], pi, SUBSEP)
            for (j = i+1; j < n; j++) {
                split(a[j], pj, SUBSEP)
                if (pj[1]+0 > pi[1]+0) {
                    tmp = a[i]; a[i] = a[j]; a[j] = tmp
                    split(a[i], pi, SUBSEP)
                }
            }
        }
        limit = (n < 10) ? n : 10
        for (i = 0; i < limit; i++) {
            split(a[i], parts, SUBSEP)
            print parts[2]
        }
    }' "$folded_actual")

    if [[ -z "$top_funcs" ]]; then
        log_warn "Could not extract top functions for annotation"
        return 1
    fi

    # Run perf annotate for each top function
    {
        echo "=== Source Annotation Report ==="
        echo "Generated: $(date)"
        echo "Perf data: $perf_data"
        echo ""

        while IFS= read -r func; do
            [[ -z "$func" ]] && continue
            echo "--- Function: $func ---"
            perf annotate --stdio -i "$perf_data" -s "$func" 2>/dev/null | head -50 || true
            echo ""
        done <<< "$top_funcs"
    } > "$annotate_file"

    if [[ -s "$annotate_file" ]]; then
        log_info "Annotation saved: $annotate_file"
    else
        log_warn "perf annotate produced no output (symbols may be stripped)"
        rm -f "$annotate_file"
    fi

    # Clean up perf.data now that annotation is done
    rm -f "$perf_data"
}
