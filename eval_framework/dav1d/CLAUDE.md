# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

dav1d is a fast, cross-platform AV1 decoder written in C99 with architecture-specific assembly acceleration (x86/NASM, ARM/GAS, PPC/VSX, RISC-V, LoongArch). Version 1.5.3, BSD 2-clause licensed.

## Build Commands

```bash
# Configure and build
meson setup build && cd build && ninja

# Run tests
meson test -v

# Run checkasm (assembly correctness tests) only
meson test --suite checkasm

# Build with specific options
meson setup build -Denable_asm=false -Dbitdepths=['8'] -Denable_tools=false

# Build documentation (requires doxygen)
meson setup build -Denable_docs=true && ninja -C build doc/html
```

Cross-compilation files are in `package/crossfiles/` for aarch64, arm, i686, x86_64, loongarch64, riscv64, wasm32/64, iPhoneOS, Android.

## Code Style Rules

- C99 without VLA, Complex, or compiler extensions (anonymous structs/unions are the only allowed extension)
- No C++ in the library
- No tabs, no trailing whitespace, no carriage returns, files must end with newline
- No invisible Unicode characters
- Commit subject lines must not end with a period
- `dav1d_` prefix for public API symbols; internal functions also use `dav1d_` prefix
- Symbol visibility is hidden by default; public API uses explicit export macros

## Architecture

### Decoder Pipeline

Entry point: `dav1d_send_data()` / `dav1d_get_picture()` in `src/lib.c` -> OBU parsing in `src/obu.c` -> frame submission in `src/decode.c` -> tile decoding with per-superblock-row task parallelism.

### Key Data Flow

`Dav1dContext` (global decoder state) -> `Dav1dFrameContext` (per-frame) -> `Dav1dTileState` (per-tile, owns CDF/entropy state) -> `Dav1dTaskContext` (per-task/thread scratch space). All defined in `src/internal.h`.

### Bitdepth Template System

Files named `*_tmpl.c` are compiled twice with `-DBITDEPTH=8` and `-DBITDEPTH=16`. The abstraction layer is in `include/common/bitdepth.h`:
- `pixel` type resolves to `uint8_t` (8bpc) or `uint16_t` (16bpc)
- `bitfn(name)` mangles to `name##_8bpc` or `name##_16bpc`
- `BF(name, suffix)` mangles to `name##_8bpc_##suffix` or `name##_16bpc_##suffix`

Affected modules: cdef, filmgrain, ipred, itx, loopfilter, looprestoration, mc, recon.

### DSP Function Dispatch

`Dav1dDSPContext` (in `src/internal.h`) holds function pointer tables for each DSP domain: film grain, intra prediction, motion compensation, inverse transform, loop filter, CDEF, loop restoration. Separate tables exist per bitdepth (`dsp[3]` for 8/10/12 bit). Each domain has an `_init` function (marked `COLD`) that populates pointers, trying arch-specific implementations first then falling back to C reference code.

### Threading Model

Two levels of parallelism (`src/thread_task.c`):
- **Frame threading**: Multiple frames decoded concurrently using `n_fc` frame contexts
- **Task threading**: Row-level parallelism within a frame — task types include TILE_ENTROPY, TILE_RECONSTRUCTION, DEBLOCK, CDEF, LOOP_RESTORATION, FG_PREP/APPLY

Platform abstraction: pthreads on POSIX, Win32 SRWLock/ConditionVariable wrapper in `src/win32/thread.c`.

### Assembly Organization

- x86: `src/x86/` — NASM `.asm` files with C wrapper `.c` files; uses `src/ext/x86inc.asm`
- ARM: `src/arm/32/` and `src/arm/64/` — GAS `.S` files for NEON/DotProd/I8MM/SVE/SVE2
- PPC: `src/ppc/` — C intrinsics (VSX/POWER9)
- RISC-V: `src/riscv/` — `.S` files for Vector extension
- LoongArch: `src/loongarch/` — `.S` files

CPU feature detection is in `src/cpu.c` with per-architecture flag retrieval.

### Large Generated/Table Files

`src/cdf.c` (CDF probability tables, ~196K lines), `src/qm.c` (quantization matrices, ~180K lines), and `src/obu.c` (~72K lines) are large but structurally straightforward table/specification files.

## Testing

- **Header tests**: Verify public headers compile as C99 (`meson test --suite headers`)
- **Checkasm**: Assembly/DSP correctness tests against C reference (`tests/checkasm/`, depends on checkasm subproject)
- **Conformance**: `tests/dav1d_argon.bash` runs against Argon test vectors (requires downloading test data)
- **Fuzzing**: `tests/libfuzzer/dav1d_fuzzer.c` with libFuzzer integration (`-Dfuzzing_engine=libfuzzer`)
- **Seek stress**: Built with `-Denable_seek_stress=true`

## Performance Testing

Test data location: `/data/chenxuqiang/eval_framework/dav1d-test-data-master/` (subdirectories: `8-bit/`, `10-bit/`, `12-bit/`).

```bash
# Basic decode benchmark (output fps and realtime multiplier)
./build/tools/dav1d -i <input.ivf> -o /dev/null

# Dump per-frame timing to file
./build/tools/dav1d -i <input.ivf> -o /dev/null --frametimes timings.txt

# Limit number of decoded frames
./build/tools/dav1d -i <input.ivf> -o /dev/null -l 100

# Specify thread count
./build/tools/dav1d -i <input.ivf> -o /dev/null --threads 4

# Restrict CPU features (e.g. test without NEON on ARM)
./build/tools/dav1d -i <input.ivf> -o /dev/null --cpumask 0

# Checkasm micro-benchmarks (per DSP function timing)
meson test --suite checkasm --benchmark -C build
# Or directly:
./build/tests/checkasm --bench
```

### Example test files (by size)

| File | Frames | Description |
|---|---|---|
| `8-bit/sframe/autostitch-480p-240p-160p.ivf` | 1800 | Largest test file (8.4M) |
| `8-bit/intra/av1-1-b8-02-allintra.ivf` | 39 | All-intra coding |
| `8-bit/data/00000001.ivf` | 3 | Standard inter |
| `10-bit/data/00000671.ivf` | 10 | 10-bit sample |
