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

Build one architecture. The default compiler is `clang`; set `CC=...` only when you want to override it:

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

`run_compare.sh` prints an aligned table directly. The comparison columns are:

- `基线/优化`: scalar baseline time divided by optimized time
- `GOTO/优化`: existing `/goto/<arch>/<case>.goto` time divided by optimized time

The external goto binaries are run with each routine's supported parameters and `-innerLoops 20`.
Examples:

- Level-1 routines use `-n`, stride options, and alpha where the routine supports it.
- `sgemv` uses `-m`, `-n`, `-trans N`, `-lda`, `-alphaR 1.0`, and `-betaR 0.0`.
- `sger` uses `-m`, `-n`, `-lda`, and `-alphaR 1.0`.
- `sgemm` uses `-m`, `-n`, `-k`, `-transa N`, `-transb N`, leading dimensions, `-alphaR 1.0`, and `-betaR 0.0`.
- `ssyrk` uses `-n`, `-k`, `-uplo U`, `-trans N`, leading dimensions, `-alphaR 1.0`, and `-betaR 0.0`.

Useful overrides:

```bash
CC=gcc ./scripts/build.sh neon
ARCH_FLAGS_SVE="-march=armv8-a+sve -msve-vector-bits=scalable" ./scripts/build.sh sve
ARCH_FLAGS_SME="-march=armv9.2-a+sme -msve-vector-bits=scalable" ./scripts/build.sh sme
CASES="sgemm,sgemv" ./scripts/run_compare.sh sve /goto
GOTO_INNER_LOOPS=50 ./scripts/run_compare.sh sme /goto
```
