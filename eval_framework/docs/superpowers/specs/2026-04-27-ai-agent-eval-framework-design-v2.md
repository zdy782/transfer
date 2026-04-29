# AI Agent 性能优化评估框架设计文档 v2

**日期**: 2026-04-27
**作者**: Claude Code
**状态**: 设计阶段
**基于**: v1 设计文档的优化版本

## 1. 项目概述

### 1.1 目标

构建一个基于 Claude Code Agent + Skills 的评估框架，用于评估 AI Agent 在 ARM 平台上进行代码性能优化的能力。

### 1.2 核心需求

- **功能正确性优先** - 优化后代码必须通过所有功能测试
- **性能提升** - 测量优化前后的性能差异
- **场景化评估** - 支持多个库、多种代码模式、不同难度等级
- **自动化** - 利用 Claude Code Agent 编排 + Subagent Skills 实现自动化分析
- **可扩展性** - 易于添加新的测试库和用例
- **结构化输出** - 生成 YAML 格式的评估结果

### 1.3 评估对象

| 库 | Perf 测试 | ARM 代码 | 构建系统 | 备注 |
|---|---|---|---|---|
| isa-l | 丰富 (23 文件) | NEON + SVE | autotools + cmake | ARM 代码最多样 |
| sleef | 少量 (3 文件) | NEON + SVE | cmake | 数学函数库 |
| ComputeLibrary | 中等 | NEON + SVE + SVE2 | cmake + bazel + scons | ARM 原生库 |
| dav1d | 最少 | NEON + SVE (56 文件) | meson | ARM 汇编量最大 |
| eigen | 丰富 (66 文件) | NEON | cmake (header-only) | 线性代数 |
| faiss | 丰富 (56 文件) | NEON + SVE | cmake | 相似度搜索 |
| libjpeg-turbo | 少量 | NEON (16 文件) | cmake | 图像处理 |
| libsodium | 无 (有第三方) | ARM Crypto | autotools | 加密库 |
| x264 | 最少 | NEON + SVE + SVE2 (29 文件) | custom configure | 视频编码 |

### 1.4 第三方 Benchmark

部分库本身缺少 perf 测试，但有第三方 benchmark 项目可用：

| 库 | 第三方 Benchmark | 路径 |
|---|---|---|
| libsodium | sodium_bench | /data/chenxuqiang/eval_framework/sodium_bench |

框架通过 `benchmark_source` 配置字段支持纳入第三方 benchmark。

## 2. 架构设计

### 2.1 目录结构

```
eval_framework/
├── .claude/
│   └── plugins/
│       └── eval-framework/
│           ├── plugin.json               # 插件元数据
│           ├── agents/
│           │   └── analyze-library.md    # 主 Agent（入口 + 编排）
│           ├── skills/
│           │   ├── discover-tests/
│           │   │   └── skill.md          # 发现 perf 测试
│           │   ├── inspect-function/
│           │   │   └── skill.md          # 分析被测函数
│           │   ├── classify-difficulty/
│           │   │   └── skill.md          # 难度分类
│           │   └── generate-yaml/
│           │       └── skill.md          # 生成 YAML
│           └── templates/
│               ├── test_case.yaml.tmpl   # 用例 YAML 模板
│               └── library_config.yaml.tmpl # 库配置模板
├── configs/                             # 库配置
│   ├── defaults.yaml
│   ├── isa-l.yaml
│   ├── sleef.yaml
│   └── ...
├── test_library/                        # 生成的用例库
│   ├── isa-l.yaml
│   ├── sleef.yaml
│   └── ...
└── docs/
    └── superpowers/specs/
        └── 2026-04-27-ai-agent-eval-framework-design-v2.md
```

### 2.2 核心设计决策

**Agent-Driven 架构：** 采用 1 个主 Agent + 4 个 Subagent Skill 的编排模式。

- 主 Agent (`analyze-library`) 负责入口、流程编排、数据传递和交互
- 子 Skill 作为 Subagent 被调度，各负责一个独立步骤
- 主 Agent 可并行调度多个 Subagent 实例处理独立任务

### 2.3 数据流

```
/analyze-library <library> [--all]
        │
        ▼
1. 加载库配置 (configs/<library>.yaml)
   - 无配置 → 使用 defaults.yaml + 交互式补充
        │
        ▼
2. 调用 discover-tests Subagent
   - 输入: 库路径、perf_test_patterns、benchmark_source
   - 输出: perf 测试文件列表（含来源标注）
        │
        ▼
3. 对每个测试文件，调用 inspect-function Subagent（可并行）
   - 输入: 测试文件路径、库路径
   - 输出: 被测函数信息（文件、函数名、代码片段、ARM 特征）
        │
        ▼
4. 对每个被测函数，调用 classify-difficulty Subagent（可并行）
   - 输入: 被测函数代码、ARM 特征
   - 输出: easy/medium/hard + 优化潜力 + 建议
        │
        ▼
5. 交互式确认：展示所有用例，用户可修改/删除/补充
        │
        ▼
6. 调用 generate-yaml Subagent
   - 输入: 确认后的用例数据
   - 输出: test_library/<library>.yaml
        │
        ▼
7. 输出总结报告
```

## 3. 组件设计

### 3.1 Plugin 元数据

```json
{
  "name": "eval-framework",
  "version": "1.0.0",
  "description": "AI Agent 性能优化评估框架",
  "author": "eval-framework-team",
  "skills": [
    "discover-tests",
    "inspect-function",
    "classify-difficulty",
    "generate-yaml"
  ],
  "agents": [
    "analyze-library"
  ]
}
```

### 3.2 主 Agent: analyze-library

**职责：** 入口 + 编排

**输入：** 库名称或 `--all`

**流程：**
1. 解析参数，确定目标库
2. 加载库配置（不存在则用 defaults.yaml + 交互补充）
3. 调用 discover-tests Subagent
4. 并行调用 inspect-function Subagent（每个测试文件一个实例）
5. 并行调用 classify-difficulty Subagent（每个函数一个实例）
6. 汇总结果，交互式确认
7. 调用 generate-yaml Subagent
8. 输出总结报告

**并行策略：**
- 步骤 4：多个测试文件的 inspect 互相独立，可并行
- 步骤 5：多个函数的 classify 互相独立，可并行
- 步骤 3 → 4、步骤 4 → 5 之间为顺序依赖

### 3.3 Subagent Skill: discover-tests

**职责：** 扫描代码库，发现所有性能测试文件

**输入：** 库配置（repo_path, perf_test_patterns, exclude_patterns, search_locations, benchmark_source, third_party_benchmarks）

**流程：**
1. 在 search_locations 下用 Glob 匹配 perf_test_patterns
2. 排除 exclude_patterns 匹配的文件
3. 若 benchmark_source 含 third_party，扫描 third_party_benchmarks 路径
4. 对每个发现的文件，提取基本信息

**输出：**

```yaml
discovered_tests:
  - file: erasure_code/gf_vect_mul_perf.c
    type: official
    size_lines: 120
  - file: crc/crc64_multibinary_perf.c
    type: official
    size_lines: 95
  - name: sodium_bench
    path: /data/chenxuqiang/eval_framework/sodium_bench
    type: third_party
    entry_files:
      - src/encryption.hpp
      - src/weld_encrypted_auth.hpp
```

### 3.4 Subagent Skill: inspect-function

**职责：** 分析测试代码，定位被测函数

**输入：** 测试文件路径、库路径

**流程：**
1. 读取测试文件，分析调用链（测试函数 → 被测 API/函数）
2. 用 LSP goToDefinition 或 Grep 定位被测函数源码位置
3. 读取被测函数代码片段
4. 识别 ARM 特征（NEON/SVE intrinsics、aarch64 汇编、`#ifdef __ARM_ARCH`）

**输出：**

```yaml
target_functions:
  - file: erasure_code/gf_vect_mul_sve.S
    name: gf_vect_mul_sve
    code_snippet: |
      gf_vect_mul_sve:
          ...
    arm_features:
      - sve
      - assembly
```

### 3.5 Subagent Skill: classify-difficulty

**职责：** 评估优化难度

**输入：** 被测函数代码、ARM 特征

**分类标准：**

| 等级 | 特征 | 优化潜力 |
|------|------|---------|
| easy | 已使用 NEON/SVE 向量化，代码已优化 | < 10% |
| medium | 纯 C/C++ 可向量化，存在循环/内存访问优化机会 | 10-30% |
| hard | 需算法级优化、复杂内存模式、多线程并行化 | > 30% |

**输出：**

```yaml
classification:
  difficulty: medium
  reason: "Pure C loop over array, no SIMD, amenable to NEON vectorization"
  optimization_potential: "medium"
  suggested_improvements:
    - "Replace scalar loop with NEON vld1q/vst1q"
```

### 3.6 Subagent Skill: generate-yaml

**职责：** 生成结构化 YAML 用例库文件

**输入：** 所有已确认的用例数据

**流程：**
1. 读取 templates/test_case.yaml.tmpl
2. 汇总所有用例，计算 metadata（总数、难度分布）
3. 填充模板，输出到 test_library/<library>.yaml

**输出：** YAML 文件路径 + 校验结果

## 4. 数据模型

### 4.1 库配置格式 (configs/\<library\>.yaml)

```yaml
library:
  name: isa-l
  description: "Intel Intelligent Storage Acceleration Library"
  repo_path: /data/chenxuqiang/eval_framework/isa-l

build:
  type: autotools  # autotools | cmake | meson | makefile
  commands:
    configure: ./autogen.sh && ./configure
    build: make
    test: make check

test_discovery:
  perf_test_patterns:
    - "*_perf.c"
    - "*_perf.cpp"
    - "*bench*.c"
  exclude_patterns:
    - "*/test/*"
    - "*/tests/*"
  search_locations:
    - erasure_code/
    - crc/
    - raid/
    - igzip/

benchmark_source: official  # official | third_party | mixed
third_party_benchmarks:     # 仅当 benchmark_source 含 third_party 时
  - name: sodium_bench
    path: /data/chenxuqiang/eval_framework/sodium_bench
    build:
      type: cmake
      commands:
        configure: cmake -B build
        build: cmake --build build
```

### 4.2 用例库格式 (test_library/\<library\>.yaml)

```yaml
library:
  name: isa-l
  description: "Intel Intelligent Storage Acceleration Library"
  platform: arm64
  analyzed_at: "2026-04-27"
  version: "master"

metadata:
  total_test_cases: 6
  difficulty_distribution:
    easy: 2
    medium: 3
    hard: 1

test_cases:
  - name: gf_vect_mul_perf
    perf_file: erasure_code/gf_vect_mul_perf.c
    benchmark_source: official
    difficulty: easy
    target_function:
      file: erasure_code/gf_multibinary.asm
      name: gf_vect_mul_avx512
      code_snippet: |
        gf_vect_mul_avx512:
            endbrand64
            vmovdqa64 (zmm0, zmm1)
      arm_features:
        - sve
        - assembly
    analysis:
      reason: "Already uses AVX-512 vectorization"
      optimization_potential: "low"
      suggested_improvements: []
```

## 5. 错误处理

| 错误场景 | 处理方式 |
|---------|---------|
| 库配置不存在 | 使用 defaults.yaml，交互式提示用户补充关键信息 |
| 第三方 benchmark 路径无效 | 跳过该 benchmark，报告警告 |
| 库无法构建 | 报告错误，标记该库为 `build_failed`，跳过 |
| discover 无结果 | 提示用户检查 patterns 配置，提供手动添加选项 |
| inspect 无法定位被测函数 | 标记为 `unresolved`，提示用户手动指定 |
| classify 不确定 | 默认归为 medium，标注 `low_confidence` |
| YAML 生成失败 | 输出原始 JSON 数据供手动转换 |

## 6. 实现计划（先小后大）

### Phase 1：插件骨架

- 创建 plugin.json
- 创建目录结构（agents/, skills/, templates/）
- 创建 defaults.yaml 配置
- 创建 YAML 模板

### Phase 2：核心 Subagent Skills

- 实现 discover-tests/skill.md
- 实现 inspect-function/skill.md
- 实现 classify-difficulty/skill.md
- 实现 generate-yaml/skill.md

### Phase 3：主 Agent + 验证

- 实现 agents/analyze-library.md
- 用 isa-l 做端到端验证
- 用 sleef 做第二个库验证

### Phase 4+（后续迭代）

- 为其余库编写 configs/
- 优化并行策略
- 评估阶段实现（Agent 执行优化、测量性能）
