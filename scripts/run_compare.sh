#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
arch="${1:-${ARCH:-all}}"
goto_root="${2:-${GOTO_ROOT:-/goto}}"
cases_default="saxpy,scopy,sdot,sgemm,sgemv,sger,sscal,ssyrk"
cases_csv="${CASES:-$cases_default}"
goto_inner_loops="${GOTO_INNER_LOOPS:-10}"

case_dims() {
  case "$1" in
    saxpy|scopy|sdot|sscal)
      printf '%s %s %s\n' 1 4096 1
      ;;
    sgemv)
      printf '%s %s %s\n' 96 128 1
      ;;
    sger)
      printf '%s %s %s\n' 64 96 1
      ;;
    sgemm)
      printf '%s %s %s\n' 48 64 64
      ;;
    ssyrk)
      printf '%s %s %s\n' 64 64 48
      ;;
    *)
      printf '%s %s %s\n' 1 1 1
      ;;
  esac
}

run_one_arch() {
  local target_arch="$1"
  QUIET=1 "$root_dir/scripts/build.sh" "$target_arch"
  local tmp_dir
  tmp_dir="$(mktemp -d)"
  trap 'rm -rf "$tmp_dir"' RETURN

  local target_arch_upper
  target_arch_upper="$(printf '%s' "$target_arch" | tr '[:lower:]' '[:upper:]')"
  printf '\n%s\n' "== ${target_arch_upper} =="
  python3 "$root_dir/scripts/parse_results.py" \
    --arch "$target_arch" --case placeholder \
    --compare-out /dev/null --goto-out /dev/null --header

  IFS=',' read -r -a case_names <<<"$cases_csv"
  for short_case in "${case_names[@]}"; do
    [[ -z "$short_case" ]] && continue
    local compare_bin goto_bin compare_out goto_out dim_m dim_n dim_k
    compare_bin="$root_dir/build/$target_arch/${short_case}.compare"
    goto_bin="$goto_root/$target_arch/${short_case}.goto"
    compare_out="$tmp_dir/${target_arch}_${short_case}_compare.out"
    goto_out="$tmp_dir/${target_arch}_${short_case}_goto.out"
    read -r dim_m dim_n dim_k < <(case_dims "$short_case")

    "$compare_bin" >"$compare_out"

    if [[ -x "$goto_bin" ]]; then
      "$goto_bin" \
        -n "$dim_n" \
        -m "$dim_m" \
        -k "$dim_k" \
        -innerLoops "$goto_inner_loops" \
        >"$goto_out" 2>&1 || true
    else
      : >"$goto_out"
    fi

    python3 "$root_dir/scripts/parse_results.py" \
      --arch "$target_arch" --case "$short_case" \
      --compare-out "$compare_out" --goto-out "$goto_out"
  done
  python3 "$root_dir/scripts/parse_results.py" \
    --arch "$target_arch" --case placeholder \
    --compare-out /dev/null --goto-out /dev/null --footer
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
