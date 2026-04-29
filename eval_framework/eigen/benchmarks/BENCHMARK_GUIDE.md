# Eigen 性能基准测试指南

## 1. 环境信息

| 项目 | 版本 |
|------|------|
| 操作系统 | openEuler 2403 SP2 (Linux 6.6.0, aarch64) |
| 编译器 | GCC 12.3.1 |
| CMake | 3.27.9 |
| Google Benchmark | 1.6.1 |
| CPU | 192 核 @ 2199 MHz |
| L1 Data | 64 KiB × 96 |
| L2 | 1024 KiB × 96 |
| L3 | 43008 KiB × 4 |
| Eigen 版本 | 5.0.1-dev |

## 2. 编译

### 2.1 前置依赖

- CMake >= 3.10
- GCC >= 10 (推荐) 或 Clang >= 14
- Google Benchmark 库（需预装，`find_package(benchmark REQUIRED)`）

安装 Google Benchmark（如未安装）：

```bash
git clone https://github.com/google/benchmark.git
cd benchmark && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc) && make install
```

### 2.2 配置与编译

```bash
# 在 Eigen 源码根目录下
mkdir -p build/benchmarks && cd build/benchmarks

# 配置（Release 模式，-O3 优化）
cmake -DCMAKE_BUILD_TYPE=Release ../../benchmarks

# 编译全部目标（并行构建）
make -j$(nproc)
```

### 2.3 编译单个目标

```bash
make bench_gemm            # 仅编译 GEMM 基准
make bench_cholesky        # 仅编译 Cholesky 基准
```

### 2.4 查看所有可用目标

```bash
make help | grep bench_
```

## 3. 运行

### 3.1 运行方式

```bash
# 运行单个基准测试（默认参数）
./Core/bench_gemm

# 限制最短运行时间（加速测试，单位：秒）
./Core/bench_gemm --benchmark_min_time=0.05

# 列出所有测试用例（不执行）
./Core/bench_gemm --benchmark_list_tests

# 运行匹配特定名称的用例（支持正则）
./Core/bench_gemm --benchmark_filter="BM_EigenGemm/512"

# 输出为 JSON 格式（方便后续分析）
./Core/bench_gemm --benchmark_format=json --benchmark_out=gemm_result.json
```

### 3.2 批量运行全部基准

```bash
BDIR=/path/to/eigen/build/benchmarks

for b in $(find $BDIR -name "bench_*" -type f -executable | sort); do
  name=$(basename $b)
  echo "=== $name ==="
  $b --benchmark_min_time=0.05 2>&1 | tee -a benchmark_results.log
  echo
done
```

### 3.3 仅运行 CI 核心子集

CI 夜间测试的核心目标列表定义在 `ci/scripts/benchmark_targets.txt`，共 15 个：

```bash
# bench_gemm, bench_gemm_double, bench_gemv, bench_dot, bench_vecadd,
# bench_trsm, bench_reductions, bench_cholesky, bench_cholesky_double,
# bench_qr, bench_svd, bench_householder
```

批量运行 CI 子集：

```bash
BDIR=/path/to/eigen/build/benchmarks
for t in bench_gemm bench_gemm_double bench_gemv bench_dot bench_vecadd \
         bench_trsm bench_reductions bench_cholesky bench_cholesky_double \
         bench_qr bench_svd; do
  exe=$(find $BDIR -name "$t" -type f -executable | head -1)
  if [ -n "$exe" ]; then
    echo "=== $t ==="
    $exe --benchmark_min_time=0.05
    echo
  fi
done
```

## 4. 基准测试目标清单

### 4.1 主 benchmarks/ 目录（48 个目标，47 个可编译）

| 子目录 | 目标名 | 测量内容 |
|--------|--------|----------|
| **Core** | `bench_gemm` | 通用矩阵乘法 C+=A*B（float，8~4096 阶方阵+非方阵），报告 GFLOPS |
| | `bench_gemm_double` | 同上，double 精度 |
| | `bench_gemv` | 矩阵-向量乘法（4 种内核路径：Gemv/GemvTrans/GemvConj/GemvAdj） |
| | `bench_vecadd` | 向量/矩阵加法（BLAS-1），报告 GB/s |
| | `bench_dot` | 点积与平方范数（float/double/complex），报告 GFLOPS |
| | `bench_trsm` | 三角求解 TRSV（单右端）和 TRSM（多右端），float/double |
| | `bench_reverse` | 矩阵和向量反转操作 |
| | `bench_move_semantics` | 拷贝语义 vs 移动语义性能对比 |
| | `bench_reductions` | 全归约（sum/prod/minCoeff/maxCoeff/mean/norm/lpNorm），float/double |
| | `bench_cwise_math` | 逐元素数学函数（30+ 种：三角/双曲/特殊函数），float/double/complex |
| | `bench_broadcasting` | 列/行归约与广播运算，float/double |
| | `bench_block_ops` | 子矩阵提取/写入（block/topRows/leftCols），float/double |
| | `bench_map` | Map vs Ref vs 拥有矩阵（求和和 GEMV），float/double |
| | `bench_diagonal` | 对角线操作：提取、对角矩阵×向量/矩阵 |
| | `bench_triangular_product` | 三角-稠密矩阵乘法 TRMM（左/右乘），float/double |
| | `bench_selfadjoint_product` | 自伴矩阵操作：SYMM 和 rankUpdate，float/double |
| | `bench_symv` | 自伴矩阵-向量乘 SYMV/HEMV，float/double |
| | `bench_trmv` | 三角矩阵-向量乘 TRMV，float/double |
| | `bench_syr` | 对称秩-1 更新 SYR，float/double |
| | `bench_syr2` | 对称秩-2 更新 SYR2，float/double |
| | `bench_construction` | 矩阵构造/初始化（setZero/setRandom/setIdentity/LinSpaced） |
| | `bench_fixed_size` | 固定大小矩阵操作（2×2~4×4 乘法/逆/行列式） |
| | `bench_fixed_size_double` | 同上，double 精度 |
| | `bench_small_matrix` | 小矩阵（2×2~8×9）全面操作，面向机器人/CV 场景 |
| **Cholesky** | `bench_cholesky` | LDLT 和 LLT 分解（float，4~1500 阶） |
| | `bench_cholesky_double` | 同上，double 精度 |
| **LU** | `bench_lu` | PartialPivLU/FullPivLU 的 compute/solve/inverse/determinant |
| | `bench_rcond` | 逆条件数估计 rcond() |
| **QR** | `bench_qr` | HouseholderQR/ColPivQR/FullPivQR/COD 分解及求解 |
| **SVD** | `bench_svd` | JacobiSVD 和 BDCSVD（ThinUV/ValuesOnly），含 QR 预条件器对比 |
| | `bench_bidiag` | 上双对角化（blocked/unblocked 路径） |
| **Eigenvalues** | `bench_eigensolver` | SelfAdjointEigenSolver 和 EigenSolver（float） |
| | `bench_eigensolver_double` | 同上，double 精度 |
| | `bench_eig33` | 3×3 自伴特征分解：迭代法 vs 闭式 computeDirect |
| **Geometry** | `bench_transforms` | Isometry/Projective 变换（作用于向量+变换复合） |
| | `bench_geometry` | 四元数/AngleAxis/Rotation2D/Transform/欧拉角/AlignedBox 等 |
| | `bench_quatmul` | 四元数乘法：向量化 vs 非向量化路径 |
| | `bench_geometry_ops` | 变换应用于点集数据（批量 N） |
| **Householder** | `bench_householder` | Householder 反射全部操作（**编译失败**，见已知问题） |
| **FFT** | `bench_fft` | FFT 正/逆变换（64~65536 点 + 非二进制尺寸），报告 MFLOPS |
| **Solvers** | `bench_dense_solvers` | 稠密求解器全览（LLT/LDLT/LU/QR/SVD/COD） |
| **Sparse** | `bench_spmv` | 稀疏矩阵-向量乘 SpMV（1000~100000 行） |
| | `bench_spmm` | 稀疏矩阵-稀疏矩阵乘 SpMM |
| | `bench_sparse_transpose` | 稀疏矩阵转置 |
| | `bench_sparse_solvers` | 稀疏求解器（SimplicialLLT/LDLT/SparseLU/SparseQR/CG/BiCGSTAB） |
| **BLAS** | `bench_blas` | Eigen BLAS C 接口（Level-1/2/3） |
| **Tuning** | `bench_blocking_sizes` | GEMM 分块大小调优（扫描 kc/mc/nc） |
| | `bench_aocl` | AOCL 场景基准（向量数学/DGEMM/特征分解） |
| | `bench_blas_gemm` | Eigen GEMM vs CBLAS GEMM 对比（需系统安装 BLAS） |

### 4.2 unsupported/benchmarks/ 目录（17 个目标）

需单独配置编译：

```bash
mkdir -p build/unsupported-benchmarks && cd build/unsupported-benchmarks
cmake -DCMAKE_BUILD_TYPE=Release ../../unsupported/benchmarks
make -j$(nproc)
```

| 模块 | 目标名 | 测量内容 |
|------|--------|----------|
| AutoDiff | `bench_autodiff` | 自动微分 vs 数值微分 vs 手动雅可比 |
| IterativeSolvers | `bench_iterative_solvers` | GMRES/MINRES/IDRS/IDRSTABL/BiCGSTABL/DGMRES |
| KroneckerProduct | `bench_kronecker` | Kronecker 乘积（稠密+稀疏） |
| MatrixFunctions | `bench_matrix_exponential` | 矩阵指数 exp()（SLAM/李群操作） |
| | `bench_matrix_logarithm` | 矩阵对数 log()（李群 log 映射） |
| | `bench_matrix_power` | 矩阵幂函数 sqrt/pow/cos/sin/cosh/sinh |
| SpecialFunctions | `bench_special_functions` | Bessel/igamma/betainc/lgamma/digamma/zeta 等 |
| Splines | `bench_splines` | 样条拟合/插值/求值/导数 |
| Tensor | `bench_contraction` | 张量收缩 |
| | `bench_convolution` | 张量卷积 |
| | `bench_reduction` | 张量归约 |
| | `bench_broadcasting` | 张量广播 |
| | `bench_shuffling` | 张量重排 |
| | `bench_tensor_fft` | 张量 FFT |
| | `bench_morphing` | 张量变形 |
| | `bench_coefficient_wise` | 张量逐元素运算 |
| | `bench_image_patch` | 图像块提取 |

## 5. 性能基线数据

以下数据在上述环境中采集（`--benchmark_min_time=0.05`），供后续性能回归对比参考。

### 5.1 BLAS 级运算

| 测试 | 规模 | 耗时 | 吞吐量 |
|------|------|------|--------|
| GEMM (float) | 32×32 | 2.2 µs | 30.0 GFLOPS |
| GEMM (float) | 128×128 | 127.5 µs | 32.9 GFLOPS |
| GEMM (float) | 256×256 | 1.0 ms | 33.2 GFLOPS |
| GEMM (float) | 512×512 | 8.1 ms | 33.1 GFLOPS |
| GEMM (float) | 1024×1024 | 65.7 ms | 32.7 GFLOPS |
| GEMM (double) | 128×128 | 250 µs | 16.8 GFLOPS |
| GEMM (double) | 512×512 | 16.3 ms | 16.6 GFLOPS |
| VecAdd | 4096 | 476 ns | 96.3 GB/s |
| VecAdd | 16384 | 1.9 µs | 97.3 GB/s |
| VecAdd | 65536 | 7.8 µs | 94.4 GB/s |
| SpMV (稀疏) | 1000×7nnz | 6.2 µs | — |
| SpMV (稀疏) | 10000×7nnz | 67.5 µs | — |
| SpMV (稀疏) | 100000×7nnz | 1.1 ms | — |

### 5.2 矩阵分解

| 测试 | 规模 | 耗时 | 吞吐量 |
|------|------|------|--------|
| LLT (Cholesky) | 64 | 10.8 µs | 8.4 GFLOPS |
| LLT (Cholesky) | 256 | 277 µs | 20.5 GFLOPS |
| LLT (Cholesky) | 1024 | 15.0 ms | 24.0 GFLOPS |
| LDLT | 64 | 14.2 µs | — |
| LDLT | 256 | 573 µs | — |
| PartialPivLU | 100×100 | 46.2 µs | — |
| PartialPivLU | 1000×1000 | 23.8 ms | — |
| FullPivLU | 100×100 | 182 µs | — |
| FullPivLU | 1000×1000 | 216 ms | — |
| HouseholderQR | 100×100 | 131 µs | — |
| HouseholderQR | 1000×1000 | 48.2 ms | — |
| ColPivHouseholderQR | 1000×1000 | 104.7 ms | — |
| JacobiSVD | 100×100 | 6.8 ms | — |
| BDCSVD | 100×100 | 1.5 ms | — |
| BDCSVD | 1000×1000 | 446.8 ms | — |
| SelfAdjointEig | 64 | 163 µs | — |
| SelfAdjointEig | 256 | 6.7 ms | — |
| SelfAdjointEig | 512 | 52.1 ms | — |

### 5.3 其他运算

| 测试 | 规模 | 耗时 | 吞吐量 |
|------|------|------|--------|
| FFT (complex\<float\>) | 256 | 1.8 µs | 5.6 GFLOPS |
| FFT (complex\<float\>) | 4096 | 45.5 µs | 5.4 GFLOPS |
| FFT (complex\<float\>) | 16384 | 213 µs | 5.4 GFLOPS |
| AngleAxis→旋转矩阵 | — | 11.2 ns | — |
| 四元数 slerp | — | ~50 ns | — |
| 变换×向量 (Isometry3f) | — | 8.3 ns | — |

## 6. 已知问题

### 6.1 bench_householder 编译失败

**现象**: 编译报错 `benchmark::Benchmark is not a member of benchmark`

**原因**: `bench_householder.cpp` 使用了 Google Benchmark 1.7+ 才引入的 `benchmark::Benchmark` 类 API，而当前安装的版本为 1.6.1，该 API 不存在。

**解决方案**: 升级 Google Benchmark 到 1.7+ 或修改源码适配 1.6.x API。

### 6.2 C++17 ABI 兼容性警告

**现象**: 编译时出现 `parameter passing for argument of type ... when C++17 is enabled changed to match C++14 in GCC 10.1`

**原因**: GCC 默认使用 C++17 ABI，但 Eigen 的某些模板函数在 C++14 和 C++17 之间参数传递方式有差异。

**影响**: 仅编译警告，不影响正确性和性能。

### 6.3 大规模测试运行时间

GEMM 4096×4096、BDCSVD 1000×1000 等大规模测试单次运行可达数秒到数十秒。批量运行全部 2911 个 benchmark case 时，建议：
- 使用 `--benchmark_min_time=0.05` 减少迭代次数
- 或使用 `--benchmark_filter` 只运行关心的用例

## 7. 结果导出与分析

### 7.1 JSON 格式导出

```bash
./Core/bench_gemm --benchmark_format=json --benchmark_out=gemm_result.json
```

### 7.2 对比两次运行结果

使用 Google Benchmark 自带的 `compare` 工具：

```bash
# 先生成两次结果
./Core/bench_gemm --benchmark_format=json --benchmark_out=run1.json
# (修改代码后重新编译)
./Core/bench_gemm --benchmark_format=json --benchmark_out=run2.json

# 对比
benchmark/tools/compare.py benchmarks run1.json run2.json
```

### 7.3 CI 回归检测

Eigen 项目自带 CI 脚本 `ci/scripts/detect_regressions.py`，可用于自动检测性能回归：

```bash
python3 ci/scripts/detect_regressions.py benchmark_result.json
```
