#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
arch="${1:-${ARCH:-all}}"
cc_bin="${CC:-cc}"
common_flags_default="-std=c11 -O3"
cases_default="saxpy,scopy,sdot,sgemm,sgemv,sger,sscal,ssyrk"
cases_csv="${CASES:-$cases_default}"

compiler_banner="$($cc_bin --version 2>/dev/null | head -n 1 || true)"
if printf '%s' "$compiler_banner" | grep -qi 'gcc'; then
  baseline_disable_default="-fno-tree-vectorize"
else
  baseline_disable_default="-fno-vectorize -fno-slp-vectorize"
fi
baseline_disable="${BASELINE_DISABLE_FLAGS:-$baseline_disable_default}"

arch_flags_for() {
  case "$1" in
    neon) printf '%s' "${ARCH_FLAGS_NEON:--march=armv8-a+simd}" ;;
    sve) printf '%s' "${ARCH_FLAGS_SVE:--march=armv8-a+sve -msve-vector-bits=scalable}" ;;
    sme) printf '%s' "${ARCH_FLAGS_SME:--march=armv9.2-a+sme -msve-vector-bits=scalable}" ;;
    *) echo "unsupported arch: $1" >&2; return 1 ;;
  esac
}

case_to_function() {
  printf 'cblas_%s' "$1"
}

build_one_arch() {
  local target_arch="$1"
  local arch_flags
  arch_flags="$(arch_flags_for "$target_arch")"
  local build_dir="$root_dir/build/$target_arch"
  mkdir -p "$build_dir"

  IFS=',' read -r -a case_names <<<"$cases_csv"
  for short_case in "${case_names[@]}"; do
    [[ -z "$short_case" ]] && continue
    local fn baseline_symbol optimized_symbol
    fn="$(case_to_function "$short_case")"
    baseline_symbol="${fn}_baseline"
    optimized_symbol="${fn}_${target_arch}_optimized"

    echo "[build] arch=$target_arch case=$short_case cc=$cc_bin"
    # shellcheck disable=SC2086
    "$cc_bin" $common_flags_default $baseline_disable \
      -D"$fn"="$baseline_symbol" \
      -c "$root_dir/src/original/${fn}_scalar.c" \
      -o "$build_dir/${short_case}_baseline.o"
    # shellcheck disable=SC2086
    "$cc_bin" $common_flags_default $arch_flags \
      -D"$fn"="$optimized_symbol" \
      -c "$root_dir/src/optimized/$target_arch/${fn}_${target_arch}.c" \
      -o "$build_dir/${short_case}_${target_arch}.o"
    # shellcheck disable=SC2086
    "$cc_bin" $common_flags_default \
      -DAPPLY_VECTORIZATION_BASELINE_FUNCTION="$baseline_symbol" \
      -DAPPLY_VECTORIZATION_OPTIMIZED_FUNCTION="$optimized_symbol" \
      -c "$root_dir/src/drivers/${fn}_driver.c" \
      -o "$build_dir/${short_case}_driver.o"
    # shellcheck disable=SC2086
    "$cc_bin" $common_flags_default \
      "$build_dir/${short_case}_baseline.o" \
      "$build_dir/${short_case}_${target_arch}.o" \
      "$build_dir/${short_case}_driver.o" \
      -o "$build_dir/${short_case}.compare"
  done
}

case "$arch" in
  all)
    for candidate_arch in neon sve sme; do
      if [[ -d "$root_dir/src/optimized/$candidate_arch" ]]; then
        build_one_arch "$candidate_arch"
      fi
    done
    ;;
  neon|sve|sme)
    build_one_arch "$arch"
    ;;
  *)
    echo "usage: $0 [neon|sve|sme|all]" >&2
    exit 1
    ;;
esac
