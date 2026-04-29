# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Intel(R) Intelligent Storage Acceleration Library (ISA-L) is a collection of optimized low-level functions for storage applications. It provides:

- **Erasure codes**: Reed-Solomon type erasure codes for any encode/decode matrix in GF(2^8)
- **CRC**: Fast cyclic redundancy check implementations (iscsi32, ieee32, t10dif, ecma64, iso64, jones64, rocksoft64 polynomials)
- **RAID**: XOR and P+Q parity operations for RAID5/6
- **Compression/Decompression**: Fast deflate-compatible data compression (igzip)

## Building

### Prerequisites

- **x86_64**: NASM 2.14.01+, gcc/clang/icc
- **aarch64**: gas 2.24+, gcc 10.1+
- **RISC-V 64**: gas 2.39+ for RVV, gcc 12.1+; portable C functions work with most compilers
- **Other**: Portable base C functions available

### Build Methods

**Autotools (recommended)**:
```bash
./autogen.sh
./configure
make
sudo make install
```

**Standard Makefile**:
```bash
make -f Makefile.unx
```

**Windows**:
```bash
nmake -f Makefile.nmake
```

**CMake (experimental)**: See `cmake/README.md`

### Build Targets

- `make check` or `make checks` - Build and run check tests
- `make tests` - Build additional unit tests
- `make perfs` - Build performance tests
- `make test` - Run unit tests
- `make perf` - Run performance tests
- `make ex` - Build examples
- `make other` - Build other utilities
- `make programs` - Build command-line programs (igzip)
- `make doc` - Build API manual

### Building Specific Units

To build only specific units:
```bash
make -f Makefile.unx units=crc
```

Available units: erasure_code, crc, raid, mem, igzip

## Architecture

### Directory Structure

- `erasure_code/` - Reed-Solomon erasure coding implementations
- `crc/` - CRC calculation functions
- `raid/` - RAID parity generation and checking (XOR, P+Q)
- `igzip/` - Deflate compression/inflation
- `mem/` - Memory operations
- `include/` - Public headers
- `programs/` - Command-line utilities (igzip)
- `tests/` - Test suite
- `tools/` - Build and test automation scripts

### Multi-Binary Dispatchers

ISA-L uses multibinary dispatchers for runtime CPU feature detection. Each unit has a multibinary wrapper (e.g., `*_multibinary.asm`) that:
1. Detects available CPU instructions at runtime
2. Dispatches to optimized implementations (AVX-512, AVX2, SSE, etc.)
3. Falls back to portable C base implementations if no acceleration available

Architecture-specific code lives in subdirectories:
- `aarch64/` - ARM 64-bit optimizations
- `riscv64/` - RISC-V 64-bit optimizations (including RVV vector extensions)
- `ppc64le/` - PowerPC 64-bit little-endian optimizations
- Base implementations in parent directories

### Key Headers

- `include/erasure_code.h` - Erasure code API
- `include/crc.h` - CRC functions
- `include/raid.h` - RAID parity API
- `include/igzip_lib.h` - Compression API
- `isa-l.h` - Master include (auto-generated)

## Testing Guide

### Quick Test Commands

```bash
# Build and run functional tests (recommended for quick verification)
make check

# Build performance tests only
make perfs

# Run performance tests (must be built first)
make perf

# Generate performance report file
make perf_report
```

### Functional Tests (make check)

Fast tests with no external dependencies. Total: 17 tests

**Erasure Code (4 tests)**:
- `gf_vect_mul_test` - Galois field vector multiplication
- `erasure_code_test` - EC encode/decode functionality
- `gf_inverse_test` - GF inverse operations
- `erasure_code_update_test` - EC update operations

**RAID (4 tests)**:
- `xor_gen_test` - XOR generation (RAID5)
- `pq_gen_test` - P+Q generation (RAID6)
- `xor_check_test` - XOR verification
- `pq_check_test` - P+Q verification

**CRC (4 tests)**:
- `crc16_t10dif_test` - CRC16 T10DIF functionality
- `crc16_t10dif_copy_test` - CRC16 with copy
- `crc64_funcs_test` - CRC64 functions
- `crc32_funcs_test` - CRC32 functions

**igzip (3 tests)**:
- `igzip_rand_test` - Random data compression
- `igzip_wrapper_hdr_test` - Header wrapper tests
- `checksum32_funcs_test` - Checksum functions

**Memory (1 test)**:
- `mem_zero_detect_test` - Zero detection functionality

**Misc (1 test)**:
- `version_test` - Library version check

### Performance Tests (make perf)

**Erasure Code (6 tests)**:
| Test | Description | Typical Output |
|------|-------------|----------------|
| `gf_vect_mul_perf` | GF vector multiplication | ~39 GB/s |
| `gf_vect_dot_prod_perf` | GF dot product | ~18 GB/s |
| `gf_vect_dot_prod_1tbl` | Single-table dot product | ~700 MB/s |
| `erasure_code_perf` | Encode/decode performance | encode: ~6-7 GB/s, decode: ~8-9 GB/s |
| `erasure_code_base_perf` | Base C implementation | ~140 MB/s |
| `erasure_code_update_perf` | Update operations | ~25-32 GB/s |

**RAID (3 tests)**:
| Test | Description | Typical Output |
|------|-------------|----------------|
| `xor_gen_perf` | XOR generation (RAID5) | ~62 GB/s |
| `pq_gen_perf` | P+Q generation (RAID6) | ~25 GB/s |
| `raid_funcs_perf` | Comprehensive RAID benchmark | XOR: ~62 GB/s, P+Q: ~25 GB/s |

**CRC (8 tests)**:
| Test | Description | Typical Output |
|------|-------------|----------------|
| `crc16_t10dif_perf` | CRC16 T10DIF | ~17 GB/s |
| `crc16_t10dif_copy_perf` | CRC16 with copy | ~17 GB/s |
| `crc16_t10dif_op_perf` | Streaming operations | ~13 GB/s |
| `crc32_ieee_perf` | CRC32 IEEE | ~17 GB/s |
| `crc32_iscsi_perf` | CRC32 iSCSI | ~23 GB/s |
| `crc32_gzip_refl_perf` | CRC32 GZIP reflected | ~23 GB/s |
| `crc64_funcs_perf` | All CRC64 variants | ~17-19 GB/s |
| `crc_funcs_perf` | Comprehensive CRC benchmark | Varies by variant |

**igzip (1 test)**:
| Test | Description | Typical Output |
|------|-------------|----------------|
| `adler32_perf` | Adler32 checksum | ~17 GB/s |

**Memory (1 test)**:
| Test | Description | Typical Output |
|------|-------------|----------------|
| `mem_zero_detect_perf` | Zero detection | ~92 GB/s |

### Running Individual Tests

```bash
# Run a specific functional test
./erasure_code/gf_vect_mul_test

# Run a specific performance test
./erasure_code/gf_vect_mul_perf

# Run igzip performance test (requires zlib)
./igzip/igzip_perf
```

### Extended Testing

```bash
# Run extended test suite with sanitizers
./tools/test_autorun.sh ext

# Run fuzz tests (requires clang with fuzzer or AFL)
./tools/test_fuzz.sh -e checked

# Run with AFL
./tools/test_fuzz.sh -e checked --afl 1 --llvm -1 -d 1
```

### Performance Report

Generate a dated performance report:

```bash
make perf_report
```

Creates `perf_report_<hostname>_<date>.perf` with all benchmark results.

## Special Considerations

### API Behavior

ISA-L functions do **not** fully validate parameters. NULL pointers and invalid inputs may cause undefined behavior. Callers are responsible for argument validation.

### Memory Management

ISA-L does not allocate memory internally (except for level 1+ compression buffers). Users must manage allocations explicitly, particularly for:
- `struct isal_zstream` level buffers for compression levels > 0
- Intermediate buffers in erasure code operations

### Threading

All library functions are single-threaded but reentrant and thread-safe. The `igzip` utility supports multi-threaded compression when built with `D="-DHAVE_THREADS"`.

### Platform-Specific Notes

- **Hygon CPUs**: The VPTERNLOGQ instruction is disabled due to compatibility issues
- **Intel CET**: Enabled by default on x86_64 Linux for security
- **RISC-V**: Vector extensions (RVV) and crypto extensions (ZBC, ZBB, ZVBC) are detected at configure time

## Development Tools

- `./tools/format.sh` - Format code with clang-format
- `./tools/check_format.sh` - Check code formatting
- `make spellcheck` - Run codespell for spelling checks
