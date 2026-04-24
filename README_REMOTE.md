# BLAS8 remote vectorization package

This package contains eight single-precision BLAS interface cases, each with:

- `src/original/`: scalar baseline source compiled with auto-vectorization disabled
- `src/optimized/neon/`: NEON optimized source
- `src/optimized/sve/`: SVE optimized source
- `src/optimized/sme/`: SME streaming optimized source
- `src/drivers/`: correctness and before/after timing drivers

Expected existing server binaries:

```text
/goto/neon/saxpy.goto
/goto/neon/scopy.goto
...
/goto/sve/sgemm.goto
/goto/sme/ssyrk.goto
```

Build one architecture:

```bash
./scripts/build.sh neon
./scripts/build.sh sve
./scripts/build.sh sme
```

Run the three-way comparison against `/goto`:

```bash
./scripts/run_compare.sh neon /goto
./scripts/run_compare.sh sve /goto
./scripts/run_compare.sh sme /goto
```

Useful overrides:

```bash
CC=clang ./scripts/build.sh sve
ARCH_FLAGS_SVE="-march=armv8-a+sve -msve-vector-bits=scalable" ./scripts/build.sh sve
ARCH_FLAGS_SME="-march=armv9.2-a+sme -msve-vector-bits=scalable" ./scripts/build.sh sme
CASES="sgemm,sgemv" ./scripts/run_compare.sh sve /goto
```

Results are written as CSV under `results/` with rows for `original`, `optimized`, and `goto`.
