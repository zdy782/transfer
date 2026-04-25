#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
arch="${1:-${ARCH:-all}}"
goto_root="${2:-${GOTO_ROOT:-/goto}}"
cases_default="saxpy,scopy,sdot,sgemm,sgemv,sger,sscal,ssyrk"
cases_csv="${CASES:-$cases_default}"
goto_inner_loops="${GOTO_INNER_LOOPS:-20}"
driver_iters="${DRIVER_ITERS:-20}"
run_bind_prefix="${RUN_BIND_PREFIX-numactl -m 31 taskset -c 575}"
run_bind_argv=()

if [[ -n "$run_bind_prefix" ]]; then
  read -r -a run_bind_argv <<<"$run_bind_prefix"
fi

run_bound() {
  if ((${#run_bind_argv[@]} > 0)); then
    "${run_bind_argv[@]}" "$@"
  else
    "$@"
  fi
}

case_dims() {
  case "$1" in
    saxpy|scopy|sdot|sscal)
      printf '%s %s %s %s\n' n=1024 1 1024 1
      printf '%s %s %s %s\n' n=4096 1 4096 1
      printf '%s %s %s %s\n' n=16384 1 16384 1
      ;;
    sgemv)
      printf '%s %s %s %s\n' 48x64 48 64 1
      printf '%s %s %s %s\n' 96x128 96 128 1
      printf '%s %s %s %s\n' 192x256 192 256 1
      ;;
    sger)
      printf '%s %s %s %s\n' 32x48 32 48 1
      printf '%s %s %s %s\n' 64x96 64 96 1
      printf '%s %s %s %s\n' 128x192 128 192 1
      ;;
    sgemm)
      printf '%s %s %s %s\n' 24x32x32 24 32 32
      printf '%s %s %s %s\n' 48x64x64 48 64 64
      printf '%s %s %s %s\n' 96x96x96 96 96 96
      ;;
    ssyrk)
      printf '%s %s %s %s\n' n32k32 32 32 32
      printf '%s %s %s %s\n' n64k48 64 64 48
      printf '%s %s %s %s\n' n128k64 128 128 64
      ;;
    *)
      printf '%s %s %s %s\n' default 1 1 1
      ;;
  esac
}

compare_args() {
  local short_case="$1"
  local dim_m="$2"
  local dim_n="$3"
  local dim_k="$4"

  case "$short_case" in
    saxpy|scopy|sdot|sscal)
      printf '%s\n' -n "$dim_n" -iters "$driver_iters"
      ;;
    sgemv|sger)
      printf '%s\n' -m "$dim_m" -n "$dim_n" -iters "$driver_iters"
      ;;
    sgemm)
      printf '%s\n' -m "$dim_m" -n "$dim_n" -k "$dim_k" -iters "$driver_iters"
      ;;
    ssyrk)
      printf '%s\n' -n "$dim_n" -k "$dim_k" -iters "$driver_iters"
      ;;
    *)
      printf '%s\n' -iters "$driver_iters"
      ;;
  esac
}

goto_args() {
  local short_case="$1"
  local dim_m="$2"
  local dim_n="$3"
  local dim_k="$4"

  case "$short_case" in
    saxpy)
      printf '%s\n' \
        -n "$dim_n" \
        -alphaR 1.25 \
        -innerLoops "$goto_inner_loops"
      ;;
    scopy|sdot)
      printf '%s\n' \
        -n "$dim_n" \
        -innerLoops "$goto_inner_loops"
      ;;
    sscal)
      printf '%s\n' \
        -n "$dim_n" \
        -alphaR -0.75 \
        -innerLoops "$goto_inner_loops"
      ;;
    sgemv)
      printf '%s\n' \
        -m "$dim_m" \
        -n "$dim_n" \
        -lda "$dim_n" \
        -betaR 0.0 \
        -innerLoops "$goto_inner_loops"
      ;;
    sger)
      printf '%s\n' \
        -m "$dim_m" \
        -n "$dim_n" \
        -lda "$dim_n" \
        -innerLoops "$goto_inner_loops"
      ;;
    sgemm)
      printf '%s\n' \
        -m "$dim_m" \
        -n "$dim_n" \
        -k "$dim_k" \
        -transb N \
        -lda "$dim_k" \
        -ldb "$dim_n" \
        -ldc "$dim_n" \
        -betaR 0.0 \
        -innerLoops "$goto_inner_loops"
      ;;
    ssyrk)
      printf '%s\n' \
        -n "$dim_n" \
        -k "$dim_k" \
        -trans T \
        -lda "$dim_k" \
        -betaR 0.0 \
        -innerLoops "$goto_inner_loops"
      ;;
    *)
      printf '%s\n' -innerLoops "$goto_inner_loops"
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
    local compare_bin goto_bin
    compare_bin="$root_dir/build/$target_arch/${short_case}.compare"
    goto_bin="$goto_root/$target_arch/${short_case}.goto"
    local dim_label dim_m dim_n dim_k
    while read -r dim_label dim_m dim_n dim_k; do
      [[ -z "$dim_label" ]] && continue
      local compare_out goto_out compare_argv=()
      compare_out="$tmp_dir/${target_arch}_${short_case}_${dim_label}_compare.out"
      goto_out="$tmp_dir/${target_arch}_${short_case}_${dim_label}_goto.out"

      local compare_arg
      while IFS= read -r compare_arg; do
        compare_argv+=("$compare_arg")
      done < <(compare_args "$short_case" "$dim_m" "$dim_n" "$dim_k")
      run_bound "$compare_bin" "${compare_argv[@]}" >"$compare_out"

      if [[ -x "$goto_bin" ]]; then
        local goto_argv=()
        local goto_arg
        while IFS= read -r goto_arg; do
          goto_argv+=("$goto_arg")
        done < <(goto_args "$short_case" "$dim_m" "$dim_n" "$dim_k")
        run_bound "$goto_bin" "${goto_argv[@]}" >"$goto_out" 2>&1 || true
      else
        : >"$goto_out"
      fi

      python3 "$root_dir/scripts/parse_results.py" \
        --arch "$target_arch" --case "$short_case" --size "$dim_label" \
        --compare-out "$compare_out" --goto-out "$goto_out"
    done < <(case_dims "$short_case")
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
