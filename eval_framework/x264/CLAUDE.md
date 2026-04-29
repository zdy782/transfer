# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

x264 is a H.264/AVC video encoder library and CLI application. It supports 8-bit and 10-bit color depths, extensive assembly optimizations for multiple architectures (x86, ARM, AArch64, PPC, MIPS, LoongArch, RISC-V), and multi-threaded encoding.

## Build Commands

```bash
# Configure with default options
./configure

# Configure with specific options
./configure --enable-debug --enable-static --disable-cli

# Build the library and CLI
make

# Build only the static library
make lib-static

# Build only the shared library
make lib-shared

# Build only the CLI
make cli

# Run assembly tests (recommended after any changes)
make checkasm      # Builds checkasm8 and checkasm10 for configured bit depths
./checkasm8        # Run 8-bit tests
./checkasm10       # Run 10-bit tests (if built)

# Build example program
make example

# Clean build artifacts
make clean
make distclean     # More thorough clean including config files

# Install
make install
make install-cli   # CLI only
make install-lib-static
make install-lib-shared
```

## Architecture

### Source Organization

- **common/** - Core encoder functionality shared across all components
  - Platform-independent C code for DCT, motion compensation, prediction, quantization, deblocking
  - Architecture-specific subdirectories (x86/, arm/, aarch64/, ppc/, mips/, loongarch/, riscv64/) for assembly optimizations
  - Assembly files use `-8.o` or `-10.o` suffix for 8-bit and 10-bit variants

- **encoder/** - Encoder-specific logic
  - `encoder.c` - Main encoding loop
  - `analyse.c` - Mode decision and analysis
  - `me.c` - Motion estimation
  - `ratecontrol.c` - Rate control (ABR, CBR, CRF, etc.)
  - `lookahead.c` - Frame type lookahead
  - `macroblock.c` - Macroblock encoding
  - `cabac.c`, `cavlc.c` - Entropy encoding
  - `set.c` - Parameter setting
  - `slicetype-cl.c` - OpenCL slice type decision

- **input/** - Input formats
  - `raw.c` - Raw YUV input
  - `y4m.c` - YUV4MPEG2 format
  - `avs.c` - Avisynth (Windows)
  - `lavf.c` - libavformat/FFmpeg
  - `ffms.c` - FFMS2
  - `timecode.c` - Timecode file support

- **output/** - Output formats
  - `raw.c` - Raw H264 output
  - `flv.c` - Flash Video
  - `matroska.c`, `matroska_ebml.c` - Matroska/WebM
  - `mp4.c` - MP4 (GPAC)
  - `mp4_lsmash.c` - MP4 (L-SMASH)

- **filters/video/** - Video processing filters
  - `source.c` - Input source handling
  - `resize.c` - Resizing
  - `crop.c` - Cropping
  - `select_every.c` - Frame selection
  - `fix_vfr_pts.c` - VFR timestamp fixing
  - `depth.c` - Bit depth conversion
  - `cache.c` - Frame caching

- **tools/** - Development tools
  - `checkasm.c` - Assembly testing framework
  - `test_x264.py` - Python regression tests using digress framework

### Build System

The Makefile uses a template-based build system:
- Source files are compiled twice for 8-bit (`*-8.o`) and 10-bit (`*-10.o`) variants
- Assembly files (`.asm`, `.S`) are architecture-specific
- `configure` generates `config.mak` with detected features and compiler settings
- `config.h` defines enabled features (HAVE_* macros)

### Bit Depth Templating

x264 uses extensive preprocessor macros for bit depth templating:
- `BIT_DEPTH` - Current bit depth (8 or 10)
- `HIGH_BIT_DEPTH` - Whether building for 10-bit
- Functions use `x264_template(name)` macro for different bit depth variants
- Pixel size varies: `SIZEOF_PIXEL` is 1 for 8-bit, 2 for 10-bit

### Threading Architecture

- Frame-level parallelism via thread pool (`common/threadpool.c`)
- Lookahead runs in separate thread(s)
- Sliced threading for frame-level parallelism
- Maximum threads: `X264_THREAD_MAX` (128)

## Testing

### Assembly Tests (checkasm)

Primary testing method for optimized code:
```bash
./checkasm8    # Test 8-bit assembly functions
./checkasm10   # Test 10-bit assembly functions
```

Tests verify C and assembly implementations produce identical results.

### Regression Tests

Python-based tests in `tools/test_x264.py`:
```bash
python tools/test_x264.py
```

Tests various encoding options and compares output bit-exactness.

### Decoder Verification

See `doc/regression_test.txt` for instructions on verifying encoder output against JM reference decoder.

## Public API

Main API is defined in `x264.h`:
- `x264_param_default_preset()` - Initialize parameters with preset/tuning
- `x264_param_apply_profile()` - Apply profile restrictions
- `x264_encoder_open()` - Open encoder
- `x264_encoder_encode()` - Encode frames
- `x264_encoder_close()` - Close encoder
- `x264_picture_alloc()` - Allocate picture structures

See `example.c` for minimal API usage example.

## Common Development Tasks

### Adding a new architecture optimization

1. Create architecture subdirectory in `common/`
2. Add assembly source files with `-8.o` and `-10.o` build targets in Makefile
3. Implement CPU detection in `common/cpu.c`
4. Add function pointer initialization in appropriate `*-c.c` file
5. Update configure script if needed

### Debugging

Enable debug symbols and disable optimization:
```bash
./configure --enable-debug
make
```

Use `--dump-yuv` option to output decoder reconstruction for debugging.

### Cross-compilation

The configure script supports cross-compilation via:
```bash
./configure --host=TRIPLET --cross-prefix=PREFIX
```

See `.gitlab-ci.yml` for examples of cross-compilation to various platforms.
