#!/usr/bin/env bash
# lib/collectors/diff.sh - Differential flame graph comparison

log_info "Generating differential flame graph..."

PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_diff"

# Copy input files to output directory for reproducibility
cp "$BASELINE" "${PREFIX_OUT}_baseline.folded"
cp "$CURRENT" "${PREFIX_OUT}_current.folded"

# Step 1: Run difffolded.pl to produce diff folded format
DIFF_FOLDED="${PREFIX_OUT}.folded"

perl "${FG_DIR}/difffolded.pl" \
    "${PREFIX_OUT}_baseline.folded" \
    "${PREFIX_OUT}_current.folded" > "$DIFF_FOLDED"

[[ -f "$DIFF_FOLDED" && -s "$DIFF_FOLDED" ]] \
    || die "difffolded.pl produced no output"

log_info "Diff folded stacks: $DIFF_FOLDED ($(wc -l < "$DIFF_FOLDED") frames)"

# Step 2: Render differential flame graph
META="Type: Diff | Baseline: $(basename "$BASELINE") | Current: $(basename "$CURRENT")"
render_svg "$DIFF_FOLDED" "${PREFIX_OUT}.svg" "$META"

# Step 3: Generate diff-specific report
generate_diff_report "$DIFF_FOLDED" "${PREFIX_OUT}.report.txt" "$META"

log_info "Diff flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: $DIFF_FOLDED and ${PREFIX_OUT}.report.txt"
