# BLAS8 remote vectorization package

This package contains eight single-precision BLAS interface cases, each with:

- `src/original/`: scalar baseline source compiled with auto-vectorization disabled
- compiler auto-vectorization build of the same scalar source for each target architecture
- `src/optimized/neon/`: NEON optimized source
- `src/optimized/sve/`: SVE optimized source
- `src/optimized/sme/`: SME streaming optimized source
- `src/drivers/`: correctness and scalar/autovec/optimized timing drivers

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

Run the four-way comparison against `/goto`:

```bash
./scripts/run_compare.sh neon /goto
./scripts/run_compare.sh sve /goto
./scripts/run_compare.sh sme /goto
```

`run_compare.sh` prints an aligned table directly. The comparison columns are:

- `基线/优化`: scalar baseline time divided by optimized time
- `自动/优化`: compiler auto-vectorized scalar time divided by optimized time
- `GOTO/优化`: existing `/goto/<arch>/<case>.goto` time divided by optimized time

The driver-reported baseline, compiler-auto-vectorized, and optimized times are averaged per kernel call.
The external goto binaries are run with each routine's supported non-default parameters and `-innerLoops 20`.
Both the local comparison driver and the external goto binary are run through the default binding prefix:

```bash
numactl -m 31 taskset -c 575
```

Examples:

- Level-1 routines use multiple `-n` values, and alpha only where it differs from the default.
- `sgemv` uses `-m`, `-n`, `-lda`, and `-betaR 0.0`.
- `sger` uses `-m`, `-n`, and `-lda`.
- `sgemm` uses `-m`, `-n`, `-k`, `-transb N`, leading dimensions, and `-betaR 0.0`.
- `ssyrk` uses `-n`, `-k`, `-trans T`, `-lda`, and `-betaR 0.0`.

Each generated compare binary also accepts runtime dimensions directly:

```bash
./build/sve/sgemm.compare -m 96 -n 96 -k 96 -iters 20
./build/sve/saxpy.compare -n 16384 -iters 20
```

Useful overrides:

```bash
CC=gcc ./scripts/build.sh neon
ARCH_FLAGS_SVE="-march=armv8-a+sve -msve-vector-bits=scalable" ./scripts/build.sh sve
ARCH_FLAGS_SME="-march=armv9.2-a+sme -msve-vector-bits=scalable" ./scripts/build.sh sme
CASES="sgemm,sgemv" ./scripts/run_compare.sh sve /goto
GOTO_INNER_LOOPS=50 ./scripts/run_compare.sh sme /goto
DRIVER_ITERS=50 ./scripts/run_compare.sh neon /goto
RUN_BIND_PREFIX= ./scripts/run_compare.sh sve /goto
RUN_BIND_PREFIX="numactl -m 31 taskset -c 575" ./scripts/run_compare.sh sme /goto
```
