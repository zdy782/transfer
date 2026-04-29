# Faiss Build Guide

## Prerequisites (openEuler)

```bash
# Install OpenBLAS
yum install -y openblas openblas-devel

# Install SWIG (for Python bindings, optional)
yum install -y swig
```

## Build Commands

### CMake Configuration

```bash
cmake -B build . \
  -DCMAKE_BUILD_TYPE=Release \
  -DFAISS_ENABLE_GPU=OFF \
  -DFAISS_ENABLE_PYTHON=OFF \
  -DBUILD_TESTING=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DBLA_VENDOR=OpenBLAS \
  -DFAISS_OPT_LEVEL=generic
```

### Compilation

```bash
make -C build -j$(nproc)
```

### Output

- Static library: `build/faiss/libfaiss.a`
- Architecture: aarch64 (ARM64)

## Notes

- `FAISS_ENABLE_PYTHON=OFF` disables Python bindings (requires Python/NumPy headers)
- `FAISS_ENABLE_GPU=OFF` disables GPU support
- `BUILD_TESTING=OFF` disables test suite
- `BLA_VENDOR=OpenBLAS` uses OpenBLAS for linear algebra
- `FAISS_OPT_LEVEL=generic` uses generic optimizations (for aarch64)

## Performance Tests

### Install Dependencies

```bash
yum install -y gflags gflags-devel
```

### CMake Configuration (with Testing)

```bash
cmake -B build . \
  -DCMAKE_BUILD_TYPE=Release \
  -DFAISS_ENABLE_GPU=OFF \
  -DBUILD_TESTING=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DBLA_VENDOR=OpenBLAS \
  -DFAISS_OPT_LEVEL=generic
```

### Compile

```bash
make -C build -j$(nproc)
```

### Run Performance Tests

```bash
# Scalar quantizer encode benchmark
/data/chenxuqiang/eval_framework/faiss/build/perf_tests/bench_scalar_quantizer_encode

# Scalar quantizer distance benchmark
/data/chenxuqiang/eval_framework/faiss/build/perf_tests/bench_scalar_quantizer_distance

# Scalar quantizer decode benchmark
/data/chenxuqiang/eval_framework/faiss/build/perf_tests/bench_scalar_quantizer_decode

# Scalar quantizer accuracy benchmark
/data/chenxuqiang/eval_framework/faiss/build/perf_tests/bench_scalar_quantizer_accuracy

# RCQ search benchmark
/data/chenxuqiang/eval_framework/faiss/build/perf_tests/bench_no_multithreading_rcq_search
```

### Run Unit Tests

```bash
make -C build test
# or run directly
/data/chenxuqiang/eval_framework/faiss/build/tests/faiss_test
```

### Test Binaries Location

- Performance tests: `build/perf_tests/`
- Unit tests: `build/tests/faiss_test`
