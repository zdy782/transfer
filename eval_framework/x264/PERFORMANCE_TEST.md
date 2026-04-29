# x264 性能测试指南

本文档记录了 x264 的性能测试方法和使用示例。

## 目录
1. [环境准备](#环境准备)
2. [checkasm 汇编测试](#checkasm-汇编测试)
3. [编码性能测试](#编码性能测试)
4. [质量指标测试](#质量指标测试)
5. [测试脚本示例](#测试脚本示例)

---

## 环境准备

### 编译 x264

```bash
# 配置和编译
./configure
make -j$(nproc)

# 编译 checkasm 测试工具
make checkasm
```

### 生成测试数据

```bash
# 方法1: 生成随机 YUV 数据 (640x480, 1帧)
dd if=/dev/urandom of=test_480p.yuv bs=$((640*480*3/2)) count=1

# 方法2: 生成多帧测试数据
dd if=/dev/urandom of=test_480p.yuv bs=$((640*480*3/2)) count=100

# 方法3: 创建 y4m 格式文件
echo "YUV4MPEG2 C420p640 640 480 30 1" > test_480p.y4m
cat test_480p.yuv >> test_480p.y4m
```

---

## checkasm 汇编测试

### 基本功能测试

```bash
# 测试 8-bit 版本
./checkasm8

# 测试 10-bit 版本
./checkasm10

# 静默模式 (只显示失败)
./checkasm8 --quiet
```

### 输出示例

```
x264: using random seed 2340927071
x264: ARMv8
 - intra pred :          [OK]
 - coeff_last :          [OK]
x264: NEON
 - pixel sad :           [OK]
 - pixel ssd :           [OK]
 - pixel satd :          [OK]
...
x264: All tests passed
```

### 基准测试模式

```bash
# 运行所有函数的基准测试
./checkasm8 --bench

# 测试特定函数
./checkasm8 --bench=pixel
./checkasm8 --bench=dct
```

**注意**: 基准测试模式可能在不同 CPU 上有兼容性问题，主要依赖 CPU 性能计数器。

---

## 编码性能测试

### 基本编码测试

```bash
# 方法1: 使用管道输入
cat input.yuv | ./x264 --preset medium -o output.h264 \
  --frames 100 --input-res 640x480 --input-csp i420 --fps 30 - /dev/null

# 方法2: 使用 y4m 格式 (自动检测分辨率)
./x264 --preset medium -o output.h264 input.y4m

# 方法3: 使用 raw 输入
./x264 --preset medium -o output.h264 --input-res 640x480 \
  --input-csp i420 --fps 30 input.yuv
```

### Preset 性能对比

```bash
# 测试不同 preset 的性能
for preset in ultrafast superfast veryfast faster fast medium slow slower veryslow placebo; do
  echo "=== Testing: $preset ==="
  cat input.yuv | ./x264 --preset $preset -o output_${preset}.h264 \
    --frames 100 --input-res 640x480 --input-csp i420 --fps 30 - /dev/null
done
```

### 输出信息解读

```
raw [info]: 640x480p 0:0 @ 30/1 fps (cfr)
x264 [info]: using cpu capabilities: ARMv8 NEON DotProd I8MM SVE SVE2
x264 [info]: profile High, level 3.0
[进度] 当前帧/总帧数, 实时fps, 实时比特率, eta 剩余时间
x264 [info]: frame I:1     Avg QP:25.00  size:303929
x264 [info]: frame P:25    Avg QP:26.44  size:    28
x264 [info]: frame B:74    Avg QP:35.34  size:    20
x264 [info]: kb/s:734.59

encoded 100 frames, 698.39 fps, 734.59 kb/s
```

---

## 质量指标测试

### PSNR/SSIM 测试

```bash
# 启用 PSNR 和 SSIM 计算
cat input.yuv | ./x264 --preset slower --psnr --ssim \
  -o output.h264 --frames 100 --input-res 640x480 \
  --input-csp i420 --fps 30 - /dev/null
```

### 输出解读

```
x264 [warning]: --psnr used with psy on: results will be invalid!
x264 [warning]: --tune psnr should be used if attempting to benchmark psnr!
x264 [info]: SSIM Mean Y:0.9991840 (30.883db)
x264 [info]: PSNR Mean Y:38.687 U:39.710 V:39.707 Avg:39.002 Global:39.002
```

**注意**: 使用 `--tune psnr` 可以禁用 psy 选项，获得更准确的 PSNR 结果。

### Tune 参数对比

```bash
# PSNR 优化
./x264 --tune psnr --psnr -o output.h264 input.yuv

# SSIM 优化
./x264 --tune ssim --ssim -o output.h264 input.yuv

# 视觉质量优化 (默认)
./x264 --tune film -o output.h264 input.yuv
```

---

## 测试脚本示例

### 完整性能测试脚本

```bash
#!/bin/bash
# x264 性能测试脚本

# 配置
RESOLUTION="640x480"
FRAMES=100
FPS=30
INPUT="test_input.yuv"

# 颜色输出
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== x264 性能测试 ===${NC}"
echo "分辨率: $RESOLUTION"
echo "帧数: $FRAMES"
echo ""

# Preset 列表
PRESETS=("ultrafast" "veryfast" "fast" "medium" "slow" "slower")

# 结果文件
RESULT_FILE="performance_results.txt"
echo "Preset, FPS, Bitrate(kb/s)" > $RESULT_FILE

# 测试每个 preset
for preset in "${PRESETS[@]}"; do
    echo -e "${YELLOW}Testing: $preset${NC}"
    
    output=$(cat $INPUT | ./x264 --preset $preset \
        -o /dev/null --frames $FRAMES \
        --input-res $RESOLUTION \
        --input-csp i420 --fps $FPS - /dev/null 2>&1)
    
    fps=$(echo "$output" | grep "encoded" | awk '{print $3}')
    bitrate=$(echo "$output" | grep "kb/s:" | awk '{print $NF}')
    
    echo "$preset, $fps, $bitrate" >> $RESULT_FILE
    echo "  FPS: $fps, Bitrate: $bitrate kb/s"
done

echo ""
echo -e "${GREEN}=== 测试完成 ===${NC}"
echo "结果保存在: $RESULT_FILE"
cat $RESULT_FILE
```

### checkasm 测试脚本

```bash
#!/bin/bash
# 汇编功能验证脚本

echo "=== x264 checkasm 测试 ==="
echo ""

echo "测试 8-bit 版本..."
if ./checkasm8 > /tmp/checkasm8.log 2>&1; then
    echo "✓ 8-bit 版本测试通过"
    grep "All tests passed" /tmp/checkasm8.log
else
    echo "✗ 8-bit 版本测试失败"
    cat /tmp/checkasm8.log
fi

echo ""
echo "测试 10-bit 版本..."
if ./checkasm10 > /tmp/checkasm10.log 2>&1; then
    echo "✓ 10-bit 版本测试通过"
    grep "All tests passed" /tmp/checkasm10.log
else
    echo "✗ 10-bit 版本测试失败"
    cat /tmp/checkasm10.log
fi
```

---

## 常用参数说明

### 预设 (Preset)
- `ultrafast` - 最快速度，最低压缩率
- `superfast` - 非常快
- `veryfast` - 很快
- `faster` - 较快
- `fast` - 快
- `medium` - 中等 (默认)
- `slow` - 慢
- `slower` - 更慢
- `veryslow` - 非常慢
- `placebo` - 最慢，最高质量

### 调优 (Tune)
- `film` - 电影内容 (默认)
- `animation` - 动画内容
- `grain` - 保留胶片颗粒
- `stillimage` - 静态图像
- `fastdecode` - 快速解码
- `zerolatency` - 零延迟直播
- `psnr` - PSNR 优化
- `ssim` - SSIM 优化

### 性能相关参数
- `--threads N` - 设置线程数
- `--sliced-threads` - 使用切片线程
- `--asm <value>` - 汇编优化 (可见/不可见)
- `--no-asm` - 禁用汇编优化

---

## 性能分析建议

1. **对比测试时保持参数一致**
   - 相同的输入文件
   - 相同的帧数和分辨率
   - 相同的线程数

2. **多次测试取平均值**
   ```bash
   for i in {1..5}; do
       ./x264 --preset medium -o test_$i.h264 input.yuv
   done
   ```

3. **使用 /usr/bin/time 获取详细资源使用**
   ```bash
   /usr/bin/time -v ./x264 --preset medium -o output.h264 input.yuv
   ```

4. **查看 CPU 特性**
   ```bash
   ./x264 --fullhelp | grep -A 20 "cpu capabilities"
   ```

---

## 故障排查

### checkasm 崩溃
- 可能是 SIMD 指令集兼容性问题
- 尝试禁用某些特性重新编译
- 检查 CPU 是否支持检测到的指令集

### 编码速度异常
- 检查线程设置: `--threads`
- 检查输入输出是否在同一个磁盘
- 使用 `--no-asm` 对比纯 C 性能

### 质量指标无效
- 使用 `--tune psnr` 或 `--tune ssim`
- 避免使用 psy 优化选项
