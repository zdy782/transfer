# BLAS8 remote vectorization package

This package contains eight single-precision BLAS interface cases, each with:

- `src/original/`: scalar baseline source compiled with auto-vectorization disabled
- compiler auto-vectorization build of the same scalar source for each target architecture
- `src/optimized/neon/`: NEON optimized source
- `src/optimized/sve/`: SVE optimized source
- `src/optimized/sme/`: SME optimized source; `sgemm` uses an SME ZA tile path
- `src/drivers/`: correctness and scalar/autovec/optimized timing drivers

The SME `sgemm` source includes weak fallback definitions for SME ABI support
routines such as `__arm_tpidr2_save`; a full platform/compiler-rt
implementation overrides them when present.

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

`run_compare.sh` prints an aligned detail table and then a per-interface summary table.
The detail table comparison columns are:

- `基线/优化`: scalar baseline time divided by optimized time
- `自动/优化`: compiler auto-vectorized scalar time divided by optimized time
- `GOTO/优化`: existing `/goto/<arch>/<case>.goto` time divided by optimized time

The driver-reported baseline, compiler-auto-vectorized, and optimized times are averaged per kernel call.
The external goto binaries are run with each routine's supported non-default parameters and `-innerLoops 20`.
If one size fails correctness or exits non-zero, the sweep records that row and continues with the remaining interfaces.
Both the local comparison driver and the external goto binary are run through the default binding prefix:

```bash
numactl -m 31 taskset -c 575
```

Examples:

Default hot-size sweep:

- Level-1 routines: `n=1024`, `8192`, `65536`, `262144`
- `sgemv` and `sger`: `64x64`, `128x128`, `256x256`, `512x512`
- `sgemm`: `32x32x32`, `64x64x64`, `96x96x96`, `128x128x128`
- `ssyrk`: `n32k32`, `n64k64`, `n128k64`, `n128k128`

GOTO arguments are generated from the same dimensions:

- Level-1 routines use `-n`, and alpha only where it differs from the default.
- `sgemv` uses `-m`, `-n`, `-lda`, and `-betaR 0.0`.
- `sger` uses `-m`, `-n`, and `-lda`.
- `sgemm` uses `-m`, `-n`, `-k`, `-transb N`, leading dimensions, and `-betaR 0.0`.
- `ssyrk` uses `-n`, `-k`, `-trans T`, `-lda`, and `-betaR 0.0`.

The summary table reports geometric mean ratios (`GM`), optimized-win counts, and the best scalar-vs-optimized size for each interface.

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
