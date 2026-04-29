# libjpeg-turbo 性能测试用例清单

## 概述

本项目性能测试全部基于 `tjbench` (TurboJPEG Benchmark) 工具。`tjbench` 源码位于 `src/tjbench.c`，通过反复执行压缩/解压缩操作来测量吞吐量。

所有 tjbench 测试在 CTest 中注册，编号覆盖 #53~#120 (shared 库) 和 #385~#452 (static 库)，共 **136 个 CTest 用例**。
此外，还有 3 个 custom target（`tjtest` / `tjtest12` / `tjtest16`）运行完整的 `test/tjbenchtest.in` 集成性能测试。

## tjbench 命令行参数说明

```
tjbench <input_image> <quality> [options]
  -precision N     : 像素精度 (8 或 12)
  -rgb             : RGB 像素格式输入
  -quiet / -q      : 静默模式，仅输出性能数据
  -tile / -ti      : 分块模式（tiled compression/decompression）
  -benchtime S     : 每项测试最小运行时间（秒），默认 5.0
  -warmup W        : 预热时间（秒），默认 1.0
  -fastupsample    : 快速上采样（仅 tilem 系列使用）
  -subsamp / -su X : 子采样模式（444/422/420/440/411/441/410/24/GRAY）
  -dct / -d F|A    : DCT 方法 (fast/accurate)
  -scale / -s X/Y  : 缩放因子
  -crop / -cr WxH+X+Y : 裁剪区域
  -yuv             : YUV 编解码模式
  -alloc / -al     : 自定义内存分配
```

## tjbench 输出指标

| 指标 | 说明 |
|------|------|
| Megapixels/sec | 吞吐量（百万像素/秒），核心性能指标 |
| Frame rate (fps) | 帧率 |
| Decompress / Decomp to YUV | 解压缩阶段指标 |
| Encode YUV | YUV 编码阶段指标 |
| Compress | 压缩阶段指标 |

**判定方式**：tjbench 执行时会输出性能数据，但 **不设性能阈值**（不会因为低于某个 Mpixels/sec 而 FAIL）。用例的 PASS/FAIL 由后续 `md5cmp` 比对输出文件 MD5 校验值决定（验证正确性）。

---

## 一、CTest 性能测试用例（136 个）

测试分 4 组配置：
- **shared** = 动态链接 turbojpeg 库（8-bit）
- **static** = 静态链接 turbojpeg 库（8-bit）
- **shared12** = 动态链接（12-bit），名称为 `tjbench12-shared-*`
- **static12** = 静态链接（12-bit），名称为 `tjbench12-static-*`

---

### 1.1 8-bit 共享库分块压缩/解压缩性能测试（#53 ~ #86，34 个）

#### 1.1.1 基础分块压缩+解压缩（2 个）

| 编号 | 用例名称 | 测试目标 | 执行命令 | 观测指标 |
|------|----------|----------|----------|----------|
| 53 | tjbench-shared-tile-cp | 准备测试图像（复制到临时目录） | `cmake -E copy_if_different testimages/testorig.ppm testout_tile.ppm` | 无（辅助步骤） |
| 54 | tjbench-shared-tile | 8-bit 分块压缩+解压缩吞吐量基准测试（RGB, 无上采样，自动 tile 大小） | `tjbench testout_tile.ppm 95 -precision 8 -rgb -quiet -tile -benchtime 0.01 -warmup 0` | Compress Megapixels/sec, Decompress Megapixels/sec |

#### 1.1.2 分块结果 MD5 校验（20 个，tile 大小: 8/16/32/64/128 × 子采样: gray/420/422/444）

| 编号 | 用例名称 | 测试目标 | 执行命令 | 观测指标 |
|------|----------|----------|----------|----------|
| 55 | tjbench-shared-tile-gray-8x8-cmp | 8x8 tile GRAY 子采样输出正确性校验 | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout_tile_GRAY_Q95_8x8.ppm` | MD5 校验通过 |
| 56 | tjbench-shared-tile-420-8x8-cmp | 8x8 tile 420 子采样输出正确性校验 | `md5cmp efca1bdf0226df01777137778cf986ec testout_tile_420_Q95_8x8.ppm` | MD5 校验通过 |
| 57 | tjbench-shared-tile-422-8x8-cmp | 8x8 tile 422 子采样输出正确性校验 | `md5cmp c300553ce1b3b90fd414ec96b62fe988 testout_tile_422_Q95_8x8.ppm` | MD5 校验通过 |
| 58 | tjbench-shared-tile-444-8x8-cmp | 8x8 tile 444 子采样输出正确性校验 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout_tile_444_Q95_8x8.ppm` | MD5 校验通过 |
| 59 | tjbench-shared-tile-gray-16x16-cmp | 16x16 tile GRAY 子采样输出正确性校验 | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout_tile_GRAY_Q95_16x16.ppm` | MD5 校验通过 |
| 60 | tjbench-shared-tile-420-16x16-cmp | 16x16 tile 420 子采样输出正确性校验 | `md5cmp 8c92c7453870d9e11c6d1dec3a8c9101 testout_tile_420_Q95_16x16.ppm` | MD5 校验通过 |
| 61 | tjbench-shared-tile-422-16x16-cmp | 16x16 tile 422 子采样输出正确性校验 | `md5cmp 6559ddb1191f5b2d3eb41081b254c4e0 testout_tile_422_Q95_16x16.ppm` | MD5 校验通过 |
| 62 | tjbench-shared-tile-444-16x16-cmp | 16x16 tile 444 子采样输出正确性校验 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout_tile_444_Q95_16x16.ppm` | MD5 校验通过 |
| 63 | tjbench-shared-tile-gray-32x32-cmp | 32x32 tile GRAY 子采样输出正确性校验 | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout_tile_GRAY_Q95_32x32.ppm` | MD5 校验通过 |
| 64 | tjbench-shared-tile-420-32x32-cmp | 32x32 tile 420 子采样输出正确性校验 | `md5cmp 3f7651872a95e469d1c7115f1b11ecef testout_tile_420_Q95_32x32.ppm` | MD5 校验通过 |
| 65 | tjbench-shared-tile-422-32x32-cmp | 32x32 tile 422 子采样输出正确性校验 | `md5cmp 58691797f4584c4c5ed5965a6bb9aec0 testout_tile_422_Q95_32x32.ppm` | MD5 校验通过 |
| 66 | tjbench-shared-tile-444-32x32-cmp | 32x32 tile 444 子采样输出正确性校验 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout_tile_444_Q95_32x32.ppm` | MD5 校验通过 |
| 67 | tjbench-shared-tile-gray-64x64-cmp | 64x64 tile GRAY 子采样输出正确性校验 | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout_tile_GRAY_Q95_64x64.ppm` | MD5 校验通过 |
| 68 | tjbench-shared-tile-420-64x64-cmp | 64x64 tile 420 子采样输出正确性校验 | `md5cmp f64c71af03fdea12363b62f1a3096aab testout_tile_420_Q95_64x64.ppm` | MD5 校验通过 |
| 69 | tjbench-shared-tile-422-64x64-cmp | 64x64 tile 422 子采样输出正确性校验 | `md5cmp 7f9e34942ae46af7b784f459ec133f5e testout_tile_422_Q95_64x64.ppm` | MD5 校验通过 |
| 70 | tjbench-shared-tile-444-64x64-cmp | 64x64 tile 444 子采样输出正确性校验 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout_tile_444_Q95_64x64.ppm` | MD5 校验通过 |
| 71 | tjbench-shared-tile-gray-128x128-cmp | 128x128 tile GRAY 子采样输出正确性校验 | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout_tile_GRAY_Q95_128x128.ppm` | MD5 校验通过 |
| 72 | tjbench-shared-tile-420-128x128-cmp | 128x128 tile 420 子采样输出正确性校验 | `md5cmp 5a5ef57517558c671bf5e75793588d69 testout_tile_420_Q95_128x128.ppm` | MD5 校验通过 |
| 73 | tjbench-shared-tile-422-128x128-cmp | 128x128 tile 422 子采样输出正确性校验 | `md5cmp 6afcb77580d85dd3eacb04b3c2bc7710 testout_tile_422_Q95_128x128.ppm` | MD5 校验通过 |
| 74 | tjbench-shared-tile-444-128x128-cmp | 128x128 tile 444 子采样输出正确性校验 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout_tile_444_Q95_128x128.ppm` | MD5 校验通过 |

#### 1.1.3 多线程快速上采样分块压缩+解压缩（12 个）

| 编号 | 用例名称 | 测试目标 | 执行命令 | 观测指标 |
|------|----------|----------|----------|----------|
| 75 | tjbench-shared-tilem-cp | 准备多线程 tile 测试图像 | `cmake -E copy_if_different testimages/testorig.ppm testout_tilem.ppm` | 无 |
| 76 | tjbench-shared-tilem | 8-bit 多线程分块压缩+解压缩吞吐量（fastupsample, 自动 tile） | `tjbench testout_tilem.ppm 95 -precision 8 -rgb -fastupsample -quiet -tile -benchtime 0.01 -warmup 0` | Compress Megapixels/sec, Decompress Megapixels/sec |
| 77 | tjbench-shared-tile-420m-8x8-cmp | 8x8 tile 420 快速上采样输出校验 | `md5cmp 66bd869b315a32a00fef1a025661ce72 testout_tilem_420_Q95_8x8.ppm` | MD5 校验通过 |
| 78 | tjbench-shared-tile-422m-8x8-cmp | 8x8 tile 422 快速上采样输出校验 | `md5cmp 55df1f96bcfb631aedeb940cf3f011f5 testout_tilem_422_Q95_8x8.ppm` | MD5 校验通过 |
| 79 | tjbench-shared-tile-420m-16x16-cmp | 16x16 tile 420 快速上采样输出校验 | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout_tilem_420_Q95_16x16.ppm` | MD5 校验通过 |
| 80 | tjbench-shared-tile-422m-16x16-cmp | 16x16 tile 422 快速上采样输出校验 | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout_tilem_422_Q95_16x16.ppm` | MD5 校验通过 |
| 81 | tjbench-shared-tile-420m-32x32-cmp | 32x32 tile 420 快速上采样输出校验 | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout_tilem_420_Q95_32x32.ppm` | MD5 校验通过 |
| 82 | tjbench-shared-tile-422m-32x32-cmp | 32x32 tile 422 快速上采样输出校验 | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout_tilem_422_Q95_32x32.ppm` | MD5 校验通过 |
| 83 | tjbench-shared-tile-420m-64x64-cmp | 64x64 tile 420 快速上采样输出校验 | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout_tilem_420_Q95_64x64.ppm` | MD5 校验通过 |
| 84 | tjbench-shared-tile-422m-64x64-cmp | 64x64 tile 422 快速上采样输出校验 | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout_tilem_422_Q95_64x64.ppm` | MD5 校验通过 |
| 85 | tjbench-shared-tile-420m-128x128-cmp | 128x128 tile 420 快速上采样输出校验 | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout_tilem_420_Q95_128x128.ppm` | MD5 校验通过 |
| 86 | tjbench-shared-tile-422m-128x128-cmp | 128x128 tile 422 快速上采样输出校验 | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout_tilem_422_Q95_128x128.ppm` | MD5 校验通过 |

---

### 1.2 12-bit 共享库分块压缩/解压缩性能测试（#87 ~ #120，34 个）

结构与 1.1 完全一致，区别：
- 测试图像为 `monkey16.png`（16-bit PNG）
- `-precision 12`
- 输出文件扩展名为 `.png`
- MD5 校验值不同

| 编号 | 用例名称 | 测试目标 | 执行命令 | 观测指标 |
|------|----------|----------|----------|----------|
| 87 | tjbench12-shared-tile-cp | 12-bit: 准备测试图像 | `cmake -E copy_if_different testimages/monkey16.png testout12_tile.png` | 无 |
| 88 | tjbench12-shared-tile | 12-bit: 分块压缩+解压缩吞吐量基准 | `tjbench testout12_tile.png 95 -precision 12 -rgb -quiet -tile -benchtime 0.01 -warmup 0` | Compress Megapixels/sec, Decompress Megapixels/sec |
| 89 | tjbench12-shared-tile-gray-8x8-cmp | 12-bit: 8x8 tile GRAY 校验 | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12_tile_GRAY_Q95_8x8.png` | MD5 校验通过 |
| 90 | tjbench12-shared-tile-420-8x8-cmp | 12-bit: 8x8 tile 420 校验 | `md5cmp 0c921be3a229b3376f5e671928c741ff testout12_tile_420_Q95_8x8.png` | MD5 校验通过 |
| 91 | tjbench12-shared-tile-422-8x8-cmp | 12-bit: 8x8 tile 422 校验 | `md5cmp f39d59bae49dd10a8532f4ab943f3e5a testout12_tile_422_Q95_8x8.png` | MD5 校验通过 |
| 92 | tjbench12-shared-tile-444-8x8-cmp | 12-bit: 8x8 tile 444 校验 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12_tile_444_Q95_8x8.png` | MD5 校验通过 |
| 93 | tjbench12-shared-tile-gray-16x16-cmp | 12-bit: 16x16 tile GRAY 校验 | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12_tile_GRAY_Q95_16x16.png` | MD5 校验通过 |
| 94 | tjbench12-shared-tile-420-16x16-cmp | 12-bit: 16x16 tile 420 校验 | `md5cmp 5e0d2e2e69ca0d1ad97797791ff1a30b testout12_tile_420_Q95_16x16.png` | MD5 校验通过 |
| 95 | tjbench12-shared-tile-422-16x16-cmp | 12-bit: 16x16 tile 422 校验 | `md5cmp f4cc229c2ceefa33052d0ca98ff79855 testout12_tile_422_Q95_16x16.png` | MD5 校验通过 |
| 96 | tjbench12-shared-tile-444-16x16-cmp | 12-bit: 16x16 tile 444 校验 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12_tile_444_Q95_16x16.png` | MD5 校验通过 |
| 97 | tjbench12-shared-tile-gray-32x32-cmp | 12-bit: 32x32 tile GRAY 校验 | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12_tile_GRAY_Q95_32x32.png` | MD5 校验通过 |
| 98 | tjbench12-shared-tile-420-32x32-cmp | 12-bit: 32x32 tile 420 校验 | `md5cmp 4949016568cf8aecc08b1af5be822bd1 testout12_tile_420_Q95_32x32.png` | MD5 校验通过 |
| 99 | tjbench12-shared-tile-422-32x32-cmp | 12-bit: 32x32 tile 422 校验 | `md5cmp 2d25e57eff9e316409523740def79876 testout12_tile_422_Q95_32x32.png` | MD5 校验通过 |
| 100 | tjbench12-shared-tile-444-32x32-cmp | 12-bit: 32x32 tile 444 校验 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12_tile_444_Q95_32x32.png` | MD5 校验通过 |
| 101 | tjbench12-shared-tile-gray-64x64-cmp | 12-bit: 64x64 tile GRAY 校验 | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12_tile_GRAY_Q95_64x64.png` | MD5 校验通过 |
| 102 | tjbench12-shared-tile-420-64x64-cmp | 12-bit: 64x64 tile 420 校验 | `md5cmp 6df139058c430b34e44b346bc5899706 testout12_tile_420_Q95_64x64.png` | MD5 校验通过 |
| 103 | tjbench12-shared-tile-422-64x64-cmp | 12-bit: 64x64 tile 422 校验 | `md5cmp a6b531c0899653baff5e20a2484a8bfa testout12_tile_422_Q95_64x64.png` | MD5 校验通过 |
| 104 | tjbench12-shared-tile-444-64x64-cmp | 12-bit: 64x64 tile 444 校验 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12_tile_444_Q95_64x64.png` | MD5 校验通过 |
| 105 | tjbench12-shared-tile-gray-128x128-cmp | 12-bit: 128x128 tile GRAY 校验 | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12_tile_GRAY_Q95_128x128.png` | MD5 校验通过 |
| 106 | tjbench12-shared-tile-420-128x128-cmp | 12-bit: 128x128 tile 420 校验 | `md5cmp 0b42e6aca0bd21b2c5846d34e652b2b6 testout12_tile_420_Q95_128x128.png` | MD5 校验通过 |
| 107 | tjbench12-shared-tile-422-128x128-cmp | 12-bit: 128x128 tile 422 校验 | `md5cmp 579ead8169a4e7144d5c3b624f4d1cc3 testout12_tile_422_Q95_128x128.png` | MD5 校验通过 |
| 108 | tjbench12-shared-tile-444-128x128-cmp | 12-bit: 128x128 tile 444 校验 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12_tile_444_Q95_128x128.png` | MD5 校验通过 |
| 109 | tjbench12-shared-tilem-cp | 12-bit: 准备多线程 tile 测试图像 | `cmake -E copy_if_different testimages/monkey16.png testout12_tilem.png` | 无 |
| 110 | tjbench12-shared-tilem | 12-bit: 多线程分块压缩+解压缩吞吐量 | `tjbench testout12_tilem.png 95 -precision 12 -rgb -fastupsample -quiet -tile -benchtime 0.01 -warmup 0` | Compress Megapixels/sec, Decompress Megapixels/sec |
| 111 | tjbench12-shared-tile-420m-8x8-cmp | 12-bit: 8x8 tile 420 快速上采样校验 | `md5cmp 540bea1d82d6964dab66003ca12823e0 testout12_tilem_420_Q95_8x8.png` | MD5 校验通过 |
| 112 | tjbench12-shared-tile-422m-8x8-cmp | 12-bit: 8x8 tile 422 快速上采样校验 | `md5cmp 70920ea8f550524fb684cefb23dfeb4d testout12_tilem_422_Q95_8x8.png` | MD5 校验通过 |
| 113 | tjbench12-shared-tile-420m-16x16-cmp | 12-bit: 16x16 tile 420 快速上采样校验 | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12_tilem_420_Q95_16x16.png` | MD5 校验通过 |
| 114 | tjbench12-shared-tile-422m-16x16-cmp | 12-bit: 16x16 tile 422 快速上采样校验 | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12_tilem_422_Q95_16x16.png` | MD5 校验通过 |
| 115 | tjbench12-shared-tile-420m-32x32-cmp | 12-bit: 32x32 tile 420 快速上采样校验 | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12_tilem_420_Q95_32x32.png` | MD5 校验通过 |
| 116 | tjbench12-shared-tile-422m-32x32-cmp | 12-bit: 32x32 tile 422 快速上采样校验 | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12_tilem_422_Q95_32x32.png` | MD5 校验通过 |
| 117 | tjbench12-shared-tile-420m-64x64-cmp | 12-bit: 64x64 tile 420 快速上采样校验 | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12_tilem_420_Q95_64x64.png` | MD5 校验通过 |
| 118 | tjbench12-shared-tile-422m-64x64-cmp | 12-bit: 64x64 tile 422 快速上采样校验 | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12_tilem_422_Q95_64x64.png` | MD5 校验通过 |
| 119 | tjbench12-shared-tile-420m-128x128-cmp | 12-bit: 128x128 tile 420 快速上采样校验 | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12_tilem_420_Q95_128x128.png` | MD5 校验通过 |
| 120 | tjbench12-shared-tile-422m-128x128-cmp | 12-bit: 128x128 tile 422 快速上采样校验 | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12_tilem_422_Q95_128x128.png` | MD5 校验通过 |

---

### 1.3 8-bit 静态库分块压缩/解压缩性能测试（#385 ~ #418，34 个）

结构与 1.1 相同，但使用 `tjbench-static` 可执行文件，输出目录为 `testout-static_*`。

| 编号 | 用例名称 | 测试目标 | 执行命令 |
|------|----------|----------|----------|
| 385 | tjbench-static-tile-cp | 静态库: 准备测试图像 | `cmake -E copy_if_different testimages/testorig.ppm testout-static_tile.ppm` |
| 386 | tjbench-static-tile | 静态库: 8-bit 分块压缩+解压缩吞吐量 | `tjbench-static testout-static_tile.ppm 95 -precision 8 -rgb -quiet -tile -benchtime 0.01 -warmup 0` |
| 387 | tjbench-static-tile-gray-8x8-cmp | 静态库: 8x8 tile GRAY | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout-static_tile_GRAY_Q95_8x8.ppm` |
| 388 | tjbench-static-tile-420-8x8-cmp | 静态库: 8x8 tile 420 | `md5cmp efca1bdf0226df01777137778cf986ec testout-static_tile_420_Q95_8x8.ppm` |
| 389 | tjbench-static-tile-422-8x8-cmp | 静态库: 8x8 tile 422 | `md5cmp c300553ce1b3b90fd414ec96b62fe988 testout-static_tile_422_Q95_8x8.ppm` |
| 390 | tjbench-static-tile-444-8x8-cmp | 静态库: 8x8 tile 444 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout-static_tile_444_Q95_8x8.ppm` |
| 391 | tjbench-static-tile-gray-16x16-cmp | 静态库: 16x16 tile GRAY | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout-static_tile_GRAY_Q95_16x16.ppm` |
| 392 | tjbench-static-tile-420-16x16-cmp | 静态库: 16x16 tile 420 | `md5cmp 8c92c7453870d9e11c6d1dec3a8c9101 testout-static_tile_420_Q95_16x16.ppm` |
| 393 | tjbench-static-tile-422-16x16-cmp | 静态库: 16x16 tile 422 | `md5cmp 6559ddb1191f5b2d3eb41081b254c4e0 testout-static_tile_422_Q95_16x16.ppm` |
| 394 | tjbench-static-tile-444-16x16-cmp | 静态库: 16x16 tile 444 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout-static_tile_444_Q95_16x16.ppm` |
| 395 | tjbench-static-tile-gray-32x32-cmp | 静态库: 32x32 tile GRAY | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout-static_tile_GRAY_Q95_32x32.ppm` |
| 396 | tjbench-static-tile-420-32x32-cmp | 静态库: 32x32 tile 420 | `md5cmp 3f7651872a95e469d1c7115f1b11ecef testout-static_tile_420_Q95_32x32.ppm` |
| 397 | tjbench-static-tile-422-32x32-cmp | 静态库: 32x32 tile 422 | `md5cmp 58691797f4584c4c5ed5965a6bb9aec0 testout-static_tile_422_Q95_32x32.ppm` |
| 398 | tjbench-static-tile-444-32x32-cmp | 静态库: 32x32 tile 444 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout-static_tile_444_Q95_32x32.ppm` |
| 399 | tjbench-static-tile-gray-64x64-cmp | 静态库: 64x64 tile GRAY | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout-static_tile_GRAY_Q95_64x64.ppm` |
| 400 | tjbench-static-tile-420-64x64-cmp | 静态库: 64x64 tile 420 | `md5cmp f64c71af03fdea12363b62f1a3096aab testout-static_tile_420_Q95_64x64.ppm` |
| 401 | tjbench-static-tile-422-64x64-cmp | 静态库: 64x64 tile 422 | `md5cmp 7f9e34942ae46af7b784f459ec133f5e testout-static_tile_422_Q95_64x64.ppm` |
| 402 | tjbench-static-tile-444-64x64-cmp | 静态库: 64x64 tile 444 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout-static_tile_444_Q95_64x64.ppm` |
| 403 | tjbench-static-tile-gray-128x128-cmp | 静态库: 128x128 tile GRAY | `md5cmp 2c3b567086e6ca0c5e6d34ad8d6f6fe8 testout-static_tile_GRAY_Q95_128x128.ppm` |
| 404 | tjbench-static-tile-420-128x128-cmp | 静态库: 128x128 tile 420 | `md5cmp 5a5ef57517558c671bf5e75793588d69 testout-static_tile_420_Q95_128x128.ppm` |
| 405 | tjbench-static-tile-422-128x128-cmp | 静态库: 128x128 tile 422 | `md5cmp 6afcb77580d85dd3eacb04b3c2bc7710 testout-static_tile_422_Q95_128x128.ppm` |
| 406 | tjbench-static-tile-444-128x128-cmp | 静态库: 128x128 tile 444 | `md5cmp 87bd58005eec73f0f313c8e38d0d793c testout-static_tile_444_Q95_128x128.ppm` |
| 407 | tjbench-static-tilem-cp | 静态库: 准备多线程 tile 图像 | `cmake -E copy_if_different testimages/testorig.ppm testout-static_tilem.ppm` |
| 408 | tjbench-static-tilem | 静态库: 8-bit 多线程分块吞吐量 | `tjbench-static testout-static_tilem.ppm 95 -precision 8 -rgb -fastupsample -quiet -tile -benchtime 0.01 -warmup 0` |
| 409 | tjbench-static-tile-420m-8x8-cmp | 静态库: 8x8 420m | `md5cmp 66bd869b315a32a00fef1a025661ce72 testout-static_tilem_420_Q95_8x8.ppm` |
| 410 | tjbench-static-tile-422m-8x8-cmp | 静态库: 8x8 422m | `md5cmp 55df1f96bcfb631aedeb940cf3f011f5 testout-static_tilem_422_Q95_8x8.ppm` |
| 411 | tjbench-static-tile-420m-16x16-cmp | 静态库: 16x16 420m | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout-static_tilem_420_Q95_16x16.ppm` |
| 412 | tjbench-static-tile-422m-16x16-cmp | 静态库: 16x16 422m | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout-static_tilem_422_Q95_16x16.ppm` |
| 413 | tjbench-static-tile-420m-32x32-cmp | 静态库: 32x32 420m | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout-static_tilem_420_Q95_32x32.ppm` |
| 414 | tjbench-static-tile-422m-32x32-cmp | 静态库: 32x32 422m | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout-static_tilem_422_Q95_32x32.ppm` |
| 415 | tjbench-static-tile-420m-64x64-cmp | 静态库: 64x64 420m | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout-static_tilem_420_Q95_64x64.ppm` |
| 416 | tjbench-static-tile-422m-64x64-cmp | 静态库: 64x64 422m | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout-static_tilem_422_Q95_64x64.ppm` |
| 417 | tjbench-static-tile-420m-128x128-cmp | 静态库: 128x128 420m | `md5cmp bf9ec2ab4875abb2efcce8f876fe2c2a testout-static_tilem_420_Q95_128x128.ppm` |
| 418 | tjbench-static-tile-422m-128x128-cmp | 静态库: 128x128 422m | `md5cmp 6502031018c2d2f69bc6353347f8df4d testout-static_tilem_422_Q95_128x128.ppm` |

---

### 1.4 12-bit 静态库分块压缩/解压缩性能测试（#419 ~ #452，34 个）

| 编号 | 用例名称 | 测试目标 | 执行命令 |
|------|----------|----------|----------|
| 419 | tjbench12-static-tile-cp | 12-bit静态库: 准备测试图像 | `cmake -E copy_if_different testimages/monkey16.png testout12-static_tile.png` |
| 420 | tjbench12-static-tile | 12-bit静态库: 分块压缩+解压缩吞吐量 | `tjbench-static testout12-static_tile.png 95 -precision 12 -rgb -quiet -tile -benchtime 0.01 -warmup 0` |
| 421 | tjbench12-static-tile-gray-8x8-cmp | 12-bit静态库: 8x8 tile GRAY | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12-static_tile_GRAY_Q95_8x8.png` |
| 422 | tjbench12-static-tile-420-8x8-cmp | 12-bit静态库: 8x8 tile 420 | `md5cmp 0c921be3a229b3376f5e671928c741ff testout12-static_tile_420_Q95_8x8.png` |
| 423 | tjbench12-static-tile-422-8x8-cmp | 12-bit静态库: 8x8 tile 422 | `md5cmp f39d59bae49dd10a8532f4ab943f3e5a testout12-static_tile_422_Q95_8x8.png` |
| 424 | tjbench12-static-tile-444-8x8-cmp | 12-bit静态库: 8x8 tile 444 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12-static_tile_444_Q95_8x8.png` |
| 425 | tjbench12-static-tile-gray-16x16-cmp | 12-bit静态库: 16x16 tile GRAY | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12-static_tile_GRAY_Q95_16x16.png` |
| 426 | tjbench12-static-tile-420-16x16-cmp | 12-bit静态库: 16x16 tile 420 | `md5cmp 5e0d2e2e69ca0d1ad97797791ff1a30b testout12-static_tile_420_Q95_16x16.png` |
| 427 | tjbench12-static-tile-422-16x16-cmp | 12-bit静态库: 16x16 tile 422 | `md5cmp f4cc229c2ceefa33052d0ca98ff79855 testout12-static_tile_422_Q95_16x16.png` |
| 428 | tjbench12-static-tile-444-16x16-cmp | 12-bit静态库: 16x16 tile 444 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12-static_tile_444_Q95_16x16.png` |
| 429 | tjbench12-static-tile-gray-32x32-cmp | 12-bit静态库: 32x32 tile GRAY | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12-static_tile_GRAY_Q95_32x32.png` |
| 430 | tjbench12-static-tile-420-32x32-cmp | 12-bit静态库: 32x32 tile 420 | `md5cmp 4949016568cf8aecc08b1af5be822bd1 testout12-static_tile_420_Q95_32x32.png` |
| 431 | tjbench12-static-tile-422-32x32-cmp | 12-bit静态库: 32x32 tile 422 | `md5cmp 2d25e57eff9e316409523740def79876 testout12-static_tile_422_Q95_32x32.png` |
| 432 | tjbench12-static-tile-444-32x32-cmp | 12-bit静态库: 32x32 tile 444 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12-static_tile_444_Q95_32x32.png` |
| 433 | tjbench12-static-tile-gray-64x64-cmp | 12-bit静态库: 64x64 tile GRAY | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12-static_tile_GRAY_Q95_64x64.png` |
| 434 | tjbench12-static-tile-420-64x64-cmp | 12-bit静态库: 64x64 tile 420 | `md5cmp 6df139058c430b34e44b346bc5899706 testout12-static_tile_420_Q95_64x64.png` |
| 435 | tjbench12-static-tile-422-64x64-cmp | 12-bit静态库: 64x64 tile 422 | `md5cmp a6b531c0899653baff5e20a2484a8bfa testout12-static_tile_422_Q95_64x64.png` |
| 436 | tjbench12-static-tile-444-64x64-cmp | 12-bit静态库: 64x64 tile 444 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12-static_tile_444_Q95_64x64.png` |
| 437 | tjbench12-static-tile-gray-128x128-cmp | 12-bit静态库: 128x128 tile GRAY | `md5cmp 999a5fd03e8bb1e0b32c9ccb29cec846 testout12-static_tile_GRAY_Q95_128x128.png` |
| 438 | tjbench12-static-tile-420-128x128-cmp | 12-bit静态库: 128x128 tile 420 | `md5cmp 0b42e6aca0bd21b2c5846d34e652b2b6 testout12-static_tile_420_Q95_128x128.png` |
| 439 | tjbench12-static-tile-422-128x128-cmp | 12-bit静态库: 128x128 tile 422 | `md5cmp 579ead8169a4e7144d5c3b624f4d1cc3 testout12-static_tile_422_Q95_128x128.png` |
| 440 | tjbench12-static-tile-444-128x128-cmp | 12-bit静态库: 128x128 tile 444 | `md5cmp 0dffd24a3a64c945139960cc0f553272 testout12-static_tile_444_Q95_128x128.png` |
| 441 | tjbench12-static-tilem-cp | 12-bit静态库: 准备多线程 tile 图像 | `cmake -E copy_if_different testimages/monkey16.png testout12-static_tilem.png` |
| 442 | tjbench12-static-tilem | 12-bit静态库: 多线程分块吞吐量 | `tjbench-static testout12-static_tilem.png 95 -precision 12 -rgb -fastupsample -quiet -tile -benchtime 0.01 -warmup 0` |
| 443 | tjbench12-static-tile-420m-8x8-cmp | 12-bit静态库: 8x8 420m | `md5cmp 540bea1d82d6964dab66003ca12823e0 testout12-static_tilem_420_Q95_8x8.png` |
| 444 | tjbench12-static-tile-422m-8x8-cmp | 12-bit静态库: 8x8 422m | `md5cmp 70920ea8f550524fb684cefb23dfeb4d testout12-static_tilem_422_Q95_8x8.png` |
| 445 | tjbench12-static-tile-420m-16x16-cmp | 12-bit静态库: 16x16 420m | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12-static_tilem_420_Q95_16x16.png` |
| 446 | tjbench12-static-tile-422m-16x16-cmp | 12-bit静态库: 16x16 422m | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12-static_tilem_422_Q95_16x16.png` |
| 447 | tjbench12-static-tile-420m-32x32-cmp | 12-bit静态库: 32x32 420m | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12-static_tilem_420_Q95_32x32.png` |
| 448 | tjbench12-static-tile-422m-32x32-cmp | 12-bit静态库: 32x32 422m | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12-static_tilem_422_Q95_32x32.png` |
| 449 | tjbench12-static-tile-420m-64x64-cmp | 12-bit静态库: 64x64 420m | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12-static_tilem_420_Q95_64x64.png` |
| 450 | tjbench12-static-tile-422m-64x64-cmp | 12-bit静态库: 64x64 422m | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12-static_tilem_422_Q95_64x64.png` |
| 451 | tjbench12-static-tile-420m-128x128-cmp | 12-bit静态库: 128x128 420m | `md5cmp 825368d2d974eed4111f25b0854643d8 testout12-static_tilem_420_Q95_128x128.png` |
| 452 | tjbench12-static-tile-422m-128x128-cmp | 12-bit静态库: 128x128 422m | `md5cmp ba85405622ea0449f33ddb5d1892853f testout12-static_tilem_422_Q95_128x128.png` |

---

## 二、完整集成性能测试（Custom Targets）

这些通过 `cmake --build build --target tjtest` 等方式运行，执行完整的 `test/tjbenchtest.in` 脚本。

### 2.1 tjtest（8-bit 完整 tjbench 测试）

```bash
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=8 -P cmakescripts/tjbenchtest.cmake
```

测试图像：`vgl_5674_0098.png`, `vgl_6434_0018a.bmp`, `vgl_6548_0026a.bmp`, `shira_bird8.bmp`

覆盖场景：

| 类别 | 测试项 | 观测指标 |
|------|--------|----------|
| 压缩基准 | 所有子采样模式 (GRAY/444/422/440/420/411/441/410/24)，fast + accurate DCT，Q=95 | Compress Megapixels/sec, 输出与 cjpeg 比对 |
| 分块压缩/解压 | 各子采样 + tile 模式，含 -nos 无平滑 | Compress + Decompress Megapixels/sec, 逐 tile 输出比对 |
| 缩放解压 | 1/8 ~ 2/1 共 15 个缩放比例 × 所有子采样 | Decompress Megapixels/sec, 输出与 djpeg 比对 |
| 部分解压 | 裁剪区域 (crop) + 缩放 | Decompress Megapixels/sec, 输出比对 |
| 变换操作 | hflip/vflip/transpose/transverse/rot90/180/270 × 所有子采样 | Transform Megapixels/sec, 输出与 jpegtran 比对 |
| 灰度变换 | 所有变换 + 转灰度 × 所有子采样 | Transform Megapixels/sec, 输出比对 |
| 变换+缩放 | 所有变换 × 所有子采样 × 15 种缩放比例 | Transform + Decompress Megapixels/sec, 输出比对 |
| YUV 编解码 | YUV 路径压缩+解压 | YUV Encode/Decode fps, Megapixels/sec |
| 自定义内存分配 | -alloc 模式下的所有编解码场景 | 同上 |
| 优化 Huffman | -optimize 熵编码模式 | 同上 |
| 渐进式 | -progressive 模式 | 同上 |
| 算术编码 | -arithmetic 模式 | 同上 |
| 无损模式 | -lossless (PSV=4) 模式 | 同上 |

### 2.2 tjtest12（12-bit tjbench 测试）

```bash
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=12 -P cmakescripts/tjbenchtest.cmake
```

测试图像：`monkey16.png`。覆盖场景与 2.1 相同但精度为 12-bit。

### 2.3 tjtest16（2~10 bit 多精度 tjbench 测试）

```bash
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=2  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=3  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=4  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=5  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=6  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=7  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=9  -P cmakescripts/tjbenchtest.cmake
cmake -DWITH_JNA=${WITH_JNA} -DPRECISION=10 -P cmakescripts/tjbenchtest.cmake
```

分别测试 precision 2~10 bit (跳过 8 和 12 bit，已由 tjtest/tjtest12 覆盖)。

---

## 三、如何运行

```bash
# 运行所有 CTest 性能测试（136 个）
cd build && ctest -R tjbench

# 运行特定配置
ctest -R tjbench-shared    # 仅 shared 8-bit (68 个)
ctest -R tjbench-static    # 仅 static 8-bit/12-bit (68 个)
ctest -R tjbench12         # 仅 12-bit (68 个)

# 运行完整集成性能测试（custom targets）
cmake --build build --target tjtest    # 8-bit 完整集成
cmake --build build --target tjtest12  # 12-bit 完整集成

# 直接运行 tjbench 获得详细性能输出
build/tjbench testimages/testorig.ppm 95 -rgb -benchtime 2.0 -warmup 1.0
build/tjbench testimages/testorig.ppm 95 -rgb -tile -benchtime 2.0 -warmup 1.0
```

## 四、总结

| 维度 | 说明 |
|------|------|
| CTest 性能用例 | **136 个**（4 配置 × 34 子用例） |
| Custom Target 集成测试 | **3 个**（tjtest, tjtest12, tjtest16） |
| 核心观测指标 | Megapixels/sec（吞吐量）、fps（帧率） |
| 覆盖精度 | 8-bit, 12-bit（以及 tjtest16 中的 2~10 bit） |
| 覆盖库类型 | shared（动态库）、static（静态库） |
| 覆盖子采样模式 | GRAY, 444, 422, 440, 420, 411, 441, 410, 24 |
| 覆盖 DCT 方法 | fast（快速）, accurate（精确） |
| 覆盖 tile 大小 | 8x8, 16x16, 32x32, 64x64, 128x128 |
| 覆盖编码模式 | 基准 Huffman, 优化 Huffman, 渐进式, 算术编码, 无损 |
| 覆盖变换操作 | hflip, vflip, transpose, transverse, rot90, rot180, rot270 |
| 覆盖缩放比例 | 1/8, 1/4, 3/8, 1/2, 5/8, 3/4, 7/8, 1, 9/8, 5/4, 11/8, 3/2, 13/8, 7/4, 15/8, 2/1 |
| PASS/FAIL 判定方式 | 输出文件 MD5 校验（性能数据为参考输出，不作为判定依据） |
