#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
arch="${1:-${ARCH:-all}}"
goto_root="${2:-${GOTO_ROOT:-/goto}}"
cases_default="saxpy,scopy,sdot,sgemm,sgemv,sger,sscal,ssyrk"
cases_csv="${CASES:-$cases_default}"

run_one_arch() {
  local target_arch="$1"
  "$root_dir/scripts/build.sh" "$target_arch"
  mkdir -p "$root_dir/results"
  local result_csv="$root_dir/results/${target_arch}_comparison.csv"
  : >"$result_csv"
  local first=1
  local tmp_dir
  tmp_dir="$(mktemp -d)"
  trap 'rm -rf "$tmp_dir"' RETURN

  IFS=',' read -r -a case_names <<<"$cases_csv"
  for short_case in "${case_names[@]}"; do
    [[ -z "$short_case" ]] && continue
    local compare_bin goto_bin compare_out goto_out
    compare_bin="$root_dir/build/$target_arch/${short_case}.compare"
    goto_bin="$goto_root/$target_arch/${short_case}.goto"
    compare_out="$tmp_dir/${target_arch}_${short_case}_compare.out"
    goto_out="$tmp_dir/${target_arch}_${short_case}_goto.out"

    echo "[run] arch=$target_arch case=$short_case original-vs-optimized"
    "$compare_bin" >"$compare_out"

    if [[ -x "$goto_bin" ]]; then
      echo "[run] arch=$target_arch case=$short_case goto=$goto_bin"
      "$goto_bin" >"$goto_out" 2>&1 || true
    else
      echo "[warn] missing goto binary: $goto_bin" >&2
      : >"$goto_out"
    fi

    if [[ "$first" -eq 1 ]]; then
      python3 "$root_dir/scripts/parse_results.py" \
        --arch "$target_arch" --case "$short_case" \
        --compare-out "$compare_out" --goto-out "$goto_out" --header >>"$result_csv"
      first=0
    else
      python3 "$root_dir/scripts/parse_results.py" \
        --arch "$target_arch" --case "$short_case" \
        --compare-out "$compare_out" --goto-out "$goto_out" >>"$result_csv"
    fi
  done
  cat "$result_csv"
  echo "[done] wrote $result_csv"
}

case "$arch" in
  all)
    for candidate_arch in neon sve sme; do
      if [[ -d "$root_dir/src/optimized/$candidate_arch" ]]; then
        run_one_arch "$candidate_arch"
      fi
    done
    ;;
  neon|sve|sme)
    run_one_arch "$arch"
    ;;
  *)
    echo "usage: $0 [neon|sve|sme|all] [goto-root]" >&2
    exit 1
    ;;
esac
