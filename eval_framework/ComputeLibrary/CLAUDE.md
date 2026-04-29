# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

The Arm Compute Library (ACL) is a collection of low-level machine learning functions optimized for Arm CPU (Cortex-A, Neoverse) and Mali GPU architectures. It provides optimized kernels for common ML operations (convolution, GEMM, pooling, activation, etc.) supporting multiple data types (FP32, FP16, BFLOAT16, INT8, UINT8).

## Build Systems

The library supports three build systems:

### SCons (Primary)

SCons is the main build system with full feature support (CPU + GPU).

```bash
# Basic build
scons build/

# Release build with specific architecture
scons arch=armv8a build/release/

# Multi-ISA build (runtime detection of CPU features)
scons multi_isa=1 build/

# Enable specific data types
scons data_type=fp16 build/

# Enable SVE/SVE2/SME features
scons arch=armv8a sve=2 build/

# Build with OpenCL (GPU) support
scons opencl=1 build/

# Build with asserts enabled
scons asserts=1 build/

# Run tests
scons test

# Clean build
scons -c
```

### CMake (Experimental, CPU only)

```bash
# Configure with preset
cmake --preset ci
cmake --preset release

# Or manual configuration
cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DARM_COMPUTE_BUILD_TESTING=ON \
    -DARM_COMPUTE_BUILD_EXAMPLES=ON

# Build
cmake --build build --config Release

# Single-ISA build with specific architecture
cmake -B build -DACL_MULTI_ISA=OFF -DACL_ARCH_ISA=armv8.6-a

# Enable SVE/SVE2/SME2
cmake -B build -DACL_BUILD_SVE=ON -DACL_BUILD_SVE2=ON -DACL_BUILD_SME2=ON
```

### Bazel (Experimental, CPU only)

```bash
# Build library
bazel build //src/...

# Run tests
bazel test //tests/...

# Build specific target
bazel build //src/cpu/operators:CpuAdd
```

## Testing

Tests use an in-house framework in `tests/framework/`:

```bash
# SCons: Build and run all tests
scons test

# SCons: Run specific test binary
./build/tests/validation/arm_compute/graph_bn_layers

# CMake: Configure with testing enabled
cmake -B build/release -DARM_COMPUTE_BUILD_TESTING=ON

# CMake: Build tests
cmake --build build/release --config Release --target arm_compute_validation
cmake --build build/release --config Release --target arm_compute_benchmark

# CMake: Run tests with ctest
cd build/release
ctest --output-on-failure

# Run specific validation test
./build/release/Release/arm_compute_validation
```

Test categories:
- `tests/validation/` - Functional validation tests (compare outputs against reference implementations)
- `tests/UNIT/` - Unit tests for specific components
- `tests/benchmark/` - Performance benchmarks

## Performance Benchmarking

The library provides multiple ways to benchmark performance:

### 1. Dedicated Benchmark Tests

Located in `tests/benchmark/`, these are specialized performance tests:

```bash
# Build benchmarks (CMake)
cmake --build build/release --config Release --target arm_compute_benchmark

# Run all benchmarks
./build/release/Release/arm_compute_benchmark --instruments=WALL_CLOCK_TIMER_MS

# List available benchmarks
./build/release/Release/arm_compute_benchmark --list-tests

# Run specific benchmark with iterations
./build/release/Release/arm_compute_benchmark --instruments=WALL_CLOCK_TIMER_MS --iterations=10 --filter="NEON/Scale"

# Run with multiple measurement instruments
./build/release/Release/arm_compute_benchmark --instruments=WALL_CLOCK_TIMER_MS,PMU_CYCLE_COUNTER
```

### 2. Validation Tests with Benchmarking

All validation tests can run with performance instrumentation:

```bash
# Build validation tests
cmake --build build/release --config Release --target arm_compute_validation

# Run validation with wall clock timing
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --iterations=5

# Run specific operation tests (e.g., GEMM)
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --filter="GEMM" --log-level=MEASUREMENTS

# Run convolution tests
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --filter="Convolution" --mode=PRECOMMIT

# Run pooling tests
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --filter="Pooling"

# Export results to JSON
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --log-format=JSON --log-file=results.json
```

### 3. Available Measurement Instruments

The test framework supports various performance measurement instruments:

| Instrument | Description | Use Case |
|------------|-------------|----------|
| `WALL_CLOCK_TIMER` | Wall clock time | General timing |
| `WALL_CLOCK_TIMER_MS` | Wall clock in milliseconds | Precise timing |
| `WALL_CLOCK_TIMESTAMPS` | High-resolution timestamps | Detailed profiling |
| `PMU_CYCLE_COUNTER` | CPU cycle counter | Low-level CPU metrics |
| `PMU_INSTRUCTION_COUNTER` | CPU instruction count | Instruction analysis |
| `SCHEDULER_TIMER` | Thread scheduler timing | Multi-threading analysis |
| `OPENCL_TIMER` | OpenCL event timing | GPU kernel timing |
| `OPENCL_MEMORY_USAGE` | OpenCL memory tracking | GPU memory analysis |

### 4. Benchmark Command Options

```bash
# Common options
--instruments=INSTRUMENTS     # Comma-separated list of measurement instruments
--iterations=N                # Number of iterations per test (default: 1)
--filter=PATTERN              # Regex to filter test cases
--mode=PRECOMMIT|NIGHTLY|ALL  # Dataset mode to use
--log-level=MEASUREMENTS      # Show only performance measurements
--log-format=JSON|PRETTY      # Output format
--log-file=PATH               # Write output to file
--threads=N                   # Number of threads to use
--test-instruments            # Test if instruments work on platform
```

### 5. Common Benchmark Workflows

```bash
# Quick performance check for specific operation
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --filter="CpuGEMM" --iterations=10

# Detailed profiling with multiple instruments
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS,PMU_CYCLE_COUNTER,SCHEDULER_TIMER --filter="Convolution"

# Export results for analysis
./build/release/Release/arm_compute_validation --instruments=WALL_CLOCK_TIMER_MS --log-format=JSON --log-file=benchmark_results.json

# Run with specific dataset mode (NIGHTLY includes more test cases)
./build/release/Release/arm_compute_benchmark --instruments=WALL_CLOCK_TIMER_MS --mode=NIGHTLY

# Test if instruments are available on current platform
./build/release/Release/arm_compute_validation --test-instruments
```

### 6. Benchmark Output Interpretation

Benchmark output shows statistics for each test:
- **AVG**: Average execution time
- **STDDEV**: Standard deviation as percentage
- **MIN**: Minimum execution time
- **MAX**: Maximum execution time
- **MEDIAN**: Median execution time

Example output:
```
Running [0] 'NEON/Scale/RunSmall@Shape=640,480:DataType=U8:...'
  Wall clock/Wall clock time: AVG=0.4642 ms, STDDEV=2.63 %, MIN=0.4440 ms, MAX=0.4830 ms, MEDIAN=0.4650 ms
```

### 7. GEMM Tuner (OpenCL)

For OpenCL GEMM performance optimization, use the GEMM tuner tools in `examples/gemm_tuner/`:

```bash
# Build GEMM tuner examples (requires SCons with OpenCL)
scons opencl=1 gemm_tuner

# The tuner consists of:
# - cl_gemm_benchmark: Shell script for running benchmarks
# - GemmTuner.py: Python script for analyzing results
# - Benchmark binaries: cl_gemm_native, cl_gemm_reshaped, etc.

# Run benchmark on device
cd examples/gemm_tuner
./cl_gemm_benchmark --help

# Use Python tuner to find optimal configurations
python GemmTuner.py --shapes=gemm_shapes.csv --configs=gemm_configs_*.csv

# Output: JSON files with optimal kernel selection and configurations
# - gemm_type_selection.json
# - gemm_config_native.json
# - gemm_config_reshaped.json
# - gemm_config_reshapedonlyrhs.json
```

Note: GEMM tuner requires SCons build system with OpenCL support. CMake builds (CPU-only) cannot use this tool.

## Architecture

### Core Components

- **`arm_compute/core/`** - Core data structures and utilities
  - `ITensorInfo.h`, `TensorInfo.h` - Tensor metadata (shape, format, quantization)
  - `Types.h` - All data types, formats, and compile-time constants
  - `Window.h` - Execution window for kernel iteration
  - `Validate.h` - Parameter validation utilities

- **`src/core/`** - Core implementation
  - `NEON/` - Arm NEON kernel implementations (CPU)
  - `CL/` - OpenCL kernel implementations (GPU)
  - `helpers/` - Helper functions

- **`src/cpu/`** - Modern CPU kernel implementation (replaces legacy NEON structure)
  - `kernels/` - CPU kernels (NEON, SVE, SVE2, SME2 implementations)
  - `operators/` - CPU operator implementations
  - `operators/assembly/` - Hand-written assembly kernels (arm_gemm, arm_conv)

- **`arm_compute/runtime/`** - Runtime execution
  - `CL/` - OpenCL runtime (CLScheduler, CLTuner, CLHelpers)
  - `NEON/` - CPU runtime (NEScheduler, memory management)
  - `CPP/` - Modern C++ runtime (experimental)
  - `IScheduler.h` - Thread scheduling interface

- **`arm_compute/graph/`** - Graph API for building neural networks
  - `GraphBuilder.h` - fluent API for constructing computation graphs
  - `nodes/` - graph node implementations (ConvolutionLayer, PoolingLayer, etc.)
  - `backends/` - backend implementations for different targets
  - `mutators/` - graph optimization passes

### Key Abstractions

1. **TensorInfo/ITensorInfo**: Describes tensor metadata (shape, data type, layout, quantization) without owning memory
2. **ITensor**: Runtime tensor with memory allocation
3. **IFunction**: Base interface for all operators/functions
4. **Window**: Describes how to iterate over tensor data for kernel execution
5. **Kernel**: Base class for all kernel implementations (INEKernel, ICLKernel)

### Backend Organization

- **NEON/`arm_compute/runtime/NEON/`** - CPU runtime using Arm NEON/SVE instructions
- **CL/`arm_compute/runtime/CL/`** - GPU runtime using OpenCL
- **CPP/`arm_compute/runtime/CPP/`** - Experimental modern C++ backend

## Development Workflow

### Pre-commit Hooks

The repository uses pre-commit hooks for code quality:

```bash
# Install pre-commit hooks
pip install pre-commit
pre-commit install

# Run manually
pre-commit run --all-files

# Specific checks
python scripts/format_code.py --check_formatting
python scripts/format_code.py --check_copyright
python scripts/generate_build_files.py --cmake  # Update CMake files
python scripts/generate_build_files.py --bazel # Update Bazel files
```

### Code Formatting

```bash
# Format code
python scripts/format_code.py

# Check formatting
python scripts/format_code.py --check_formatting

# clang-format (configured in .clang-format)
clang-format -i file.cpp
```

### Adding New Kernels

When adding a new kernel or operator:

1. Add kernel implementation to `src/cpu/kernels/` (modern) or `src/core/NEON/kernels/` (legacy)
2. Add operator wrapper to `src/cpu/operators/` or `arm_compute/runtime/NEON/functions/`
3. Update `NEKernels.h` or appropriate headers
4. Add validation tests to `tests/validation/NEON/` or `tests/validation/cpu/`
5. Run `python scripts/include_functions_kernels.py` to update build files

### Graph API Usage

```cpp
#include "arm_compute/graph/GraphBuilder.h"

using namespace arm_compute::graph;

// Create graph
Graph graph;

// Add layers
graph << Tensor(TensorInfo(shape, Format::F32))
     << ConvolutionLayer(...)
     << ActivationLayer(...)
     << PoolingLayer(...);

// Configure and run
graph.run();
```

## Important Notes

- **Multi-ISA builds**: Enable runtime CPU feature detection for maximum compatibility. Use `multi_isa=1` (SCons) or `-DACL_MULTI_ISA=ON` (CMake)
- **Assembly kernels**: Hand-written kernels in `src/cpu/kernels/assembly/` are auto-generated or vendored from external projects (arm_gemm, KleidiAI)
- **Quantization**: The library heavily uses asymmetric quantization (QASYMM8, QASYMM8_SIGNED). Check `arm_compute/core/QuantizationInfo.h`
- **Memory management**: Use `MemoryManager` and `BlobMemoryPool` for efficient memory allocation in graphs
- **Thread safety**: The library is thread-safe for concurrent execution of different graphs, but not for modifying a single graph concurrently
