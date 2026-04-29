# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SLEEF (SIMD Library for Evaluating Elementary Functions) implements vectorized C standard math functions and DFT subroutines. Version 4.0.0, licensed under Boost Software License v1.0.

## Build Commands

Out-of-source builds are **mandatory** (in-source builds are explicitly forbidden).

```bash
# Full build
mkdir -p build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
cmake --build . -j --clean-first

# Run all tests
ctest -j $(nproc)

# Install
cmake --install .
```

For verbose CMake configuration output, add `-DSLEEF_SHOW_CONFIG=ON`.

## Key CMake Options

| Option | Default | Purpose |
|--------|---------|---------|
| `SLEEF_BUILD_LIBM` | ON | Core math library (libsleef) |
| `SLEEF_BUILD_DFT` | OFF | DFT library (libsleefdft) |
| `SLEEF_BUILD_QUAD` | OFF | Quad-precision library (libsleefquad) |
| `SLEEF_BUILD_TESTS` | ON | Build test executables |
| `SLEEF_BUILD_INLINE_HEADERS` | OFF | Inline function headers |
| `SLEEF_ENABLE_LTO` | OFF | Link-time optimization |
| `SLEEF_ENABLE_ASAN` | OFF | Address sanitizer |
| `SLEEF_ENABLE_SSE2/AVX2/AVX512F/SVE/VSX/VXE/RVVM1/RVVM2` | Various | SIMD extension enables |
| `SLEEF_ENABLE_CUDA` | OFF | CUDA support |
| `SLEEF_ENABLE_OPENMP` | ON | OpenMP (required for DFT) |
| `SLEEF_ENABLE_TLFLOAT` | ON | TLFloat library for reference computations |

For static libraries: `-DBUILD_SHARED_LIBS=OFF`.

## Architecture

### Three Libraries

1. **libsleef** (`src/libm/`) — Vectorized math functions (double, float, long double). Always built by default.
2. **libsleefdft** (`src/dft/`, C++) — DFT subroutines. Requires OpenMP. Enable with `-DSLEEF_BUILD_DFT=ON`.
3. **libsleefquad** (`src/quad/`) — Quad-precision math. Enable with `-DSLEEF_BUILD_QUAD=ON`.

### Build-Time Code Generation

SLEEF heavily uses build-time code generation to produce architecture-specific headers and dispatchers. This is the key architectural pattern to understand:

- **mkrename** — Generates `renameXXX.h` files that remap generic function names (e.g., `xsincos`) to architecture-specific names (e.g., `Sleef_sincosf4_u10avx2`)
- **mkdisp** — Generates dispatcher C code that selects the best SIMD implementation at runtime
- **mkalias** — Generates alias headers for SIMD extensions supporting multiple vector widths
- **mkunroll** (DFT) — Generates unrolled DFT kernel C++ source files
- **addSuffix** — Adds architecture-specific suffixes to inline header function names

The `.org` and `.org.in` template files (in `src/libm/`, `src/quad/`, `src/dft/`) are combined with generated code at build time to produce the final `sleef.h`, `sleefquad.h`, and `sleefinline_*.h` headers.

### Source Layout

- `src/arch/` — Architecture-specific SIMD helper headers (helperavx2.h, helpersve.h, etc.)
- `src/common/` — Shared utilities: double-double arithmetic (`dd.h`), double-float (`df.h`), test utilities, internal SHA-256 (`psha2`)
- `src/libm/` — Core math implementations (`sleefsimddp.c`, `sleefsimdsp.c`) + code generation tools
- `src/dft/` — DFT implementation in C++ with template-based kernel generation
- `src/quad/` — Quad-precision math, mirrors `src/libm/` structure
- `src/gencoef/` — Standalone coefficient generation tools (own Makefile, not in CMake build)
- `include/` — Public headers (`sleef.h` is generated at build time)
- `docs/` — Website docs (XHTML) and example source code
- `toolchains/` — Cross-compilation CMake toolchain files (aarch64, ppc64el, s390x, riscv64)
- `submodules/tlfloat/` — TLFloat library (auto-downloaded if missing)

### Supported SIMD Extensions

Mainline: x86 AVX2/AVX512F, AArch64 AdvSIMD. Experimental: SSE2, VSX/VSX3, VXE/VXE2. Unmaintained: SVE, RVVM1/RVVM2. Generic fallback: PUREC_SCALAR, PURECFMA_SCALAR.

## Testing

Tests use CTest. Test executables are in `src/libm-tester/`, `src/dft-tester/`, `src/quad-tester/`.

Key testers in `src/libm-tester/`:
- **tester/tester2** — MPFR-based reference comparison (requires libmpfr + libgmp)
- **tester3** — Hash-based testing using SHA-256 of expected results from `hash_finz.txt` (requires OpenSSL or falls back to internal psha2)
- **tester4** — TLFloat-based reference (requires TLFloat)
- **iut/iutsimd** — Implementation-under-test runners, compiled per SIMD extension

Tests are registered per SIMD extension (e.g., `iutadvsimd`, `tester3advsimd`, `tester4advsimd`).

Optional test dependencies: libssl/libcrypto (OpenSSL), libmpfr, libgmp, libfftw3.

## Requirements

- CMake >= 3.18
- C and C++ compilers of the **same version** (C99, C++20)
- Ninja recommended
- TLFloat >= 1.16.0 (auto-downloaded via git submodule)
