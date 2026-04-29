# Sodium Bench 测试指南

## 编译

```bash
cd build
cmake ..
make
```

## 快速测试

### 1. 查看帮助信息
```bash
./sodium_bench --help
```

### 2. 列出所有可用的基准测试
```bash
./sodium_bench --benchmark_list_tests=true
```

### 3. 快速功能测试（单次重复）
```bash
./sodium_bench --benchmark_filter="BM_weld_encrypt/64/1/manual_time" --benchmark_repetitions=1
```

### 4. 查看特定测试类型的基准测试
```bash
# 加密相关测试
./sodium_bench --benchmark_list_tests=true | grep "BM_" | grep -i "encrypt"

# 认证相关测试
./sodium_bench --benchmark_list_tests=true | grep "BM_" | grep -i "auth"

# 哈希相关测试
./sodium_bench --benchmark_list_tests=true | grep "BM_" | grep -i "hash"
```

## 完整测试

### 运行所有基准测试
```bash
./sodium_bench
```

### 运行特定测试（使用正则过滤）
```bash
# 示例：运行 weld_encrypt 测试
./sodium_bench --benchmark_filter="BM_weld_encrypt"

# 示例：运行多线程测试
./sodium_bench --benchmark_filter="BM_multithread"
```

### 自定义测试参数
```bash
# 设置最小运行时间（秒）
./sodium_bench --benchmark_min_time=1.0

# 设置重复次数
./sodium_bench --benchmark_repetitions=3

# 输出为 JSON 格式
./sodium_bench --benchmark_format=json --benchmark_out=results.json

# 输出为 CSV 格式
./sodium_bench --benchmark_format=csv --benchmark_out=results.csv
```

## 输出文件位置

测试结果可以保存到 `results/` 目录：
```bash
./sodium_bench --benchmark_out=results/benchmark_results.json
```

## 常用选项

| 选项 | 说明 |
|------|------|
| `--benchmark_list_tests={true\|false}` | 列出所有测试 |
| `--benchmark_filter=<regex>` | 使用正则过滤测试 |
| `--benchmark_min_time=<min_time>` | 最小运行时间（秒） |
| `--benchmark_repetitions=<num>` | 重复次数 |
| `--benchmark_format=<console\|json\|csv>` | 输出格式 |
| `--benchmark_out=<filename>` | 输出文件路径 |

## 编译环境

**已安装的依赖：**
- libsodium-devel 1.0.19
- boost-devel 1.83.0
- google-benchmark-devel 1.6.1
- gcc 12.3.1

**编译日期：** 2026-04-27
