#!/usr/bin/env bash
# lib/collectors/generic.sh - Generic folded stack file import

[[ -z "$INPUT_FILE" ]] && die "Generic mode requires -f <folded_file>"
[[ -f "$INPUT_FILE" ]] || die "Input file not found: $INPUT_FILE"

PREFIX_OUT="${OUTPUT_DIR}/${PREFIX}_generic"
META="Type: Generic | Input: ${INPUT_FILE}"

log_info "Importing folded stacks from: $INPUT_FILE"

# Copy folded file to output
cp "$INPUT_FILE" "${PREFIX_OUT}.folded"

# Render SVG
render_svg "${PREFIX_OUT}.folded" "${PREFIX_OUT}.svg" "$META"

# Generate report
generate_report "${PREFIX_OUT}.folded" "${PREFIX_OUT}.report.txt" "$META"

log_info "Generic flame graph: ${PREFIX_OUT}.svg"
log_info "AI data: ${PREFIX_OUT}.folded and ${PREFIX_OUT}.report.txt"
