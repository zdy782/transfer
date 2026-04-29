# AI Agent 性能优化评估框架设计文档

**日期**: 2026-04-27
**作者**: Claude Code
**状态**: 设计阶段

## 1. 项目概述

### 1.1 目标

构建一个基于 Claude Code Skills 的评估框架，用于评估 AI Agent 在 ARM 平台上进行代码性能优化的能力。

### 1.2 核心需求

- **功能正确性优先** - 优化后代码必须通过所有功能测试
- **性能提升** - 测量优化前后的性能差异
- **场景化评估** - 支持多个库、多种代码模式、不同难度等级
- **自动化** - 利用 Claude Code Skills 实现自动化分析
- **可扩展性** - 易于添加新的测试库和用例
- **结构化输出** - 生成 JSON/YAML 格式的评估结果

### 1.3 评估对象

当前包含的测试库：
- ComputeLibrary (ARM)
- dav1d (视频解码)
- eigen (线性代数)
- faiss (相似度搜索)
- isa-l (存储加速)
- libjpeg-turbo (JPEG)
- libsodium (加密)
- sleef (数学函数)
- x264 (视频编码)

## 2. 架构设计

### 2.1 目录结构

```
eval_framework/
├── .claude/
│   └── plugins/
│       └── eval-framework/           # 评估框架插件
│           ├── plugin.json           # 插件元数据
│           ├── skills/               # Skills 定义
│           │   ├── analyze-library.md
│           │   ├── discover-tests.md
│           │   ├── inspect-function.md
│           │   ├── classify-difficulty.md
│           │   └── generate-yaml.md
│           ├── hooks/                # 可选的 hooks
│           │   ├── PreToolUse/       # 工具使用前钩子
│           │   └── PostToolUse/      # 工具使用后钩子
│           └── templates/            # YAML 模板
│               └── test_case.yaml.tmpl
├── test_library/                    # 生成的用例库
│   ├── isa-l.yaml
│   ├── sleef.yaml
│   └── ...
├── configs/                         # 库配置
│   ├── isa-l.yaml
│   ├── sleef.yaml
│   └── defaults.yaml                # 默认配置
└── docs/
    └── superpowers/specs/
        └── 2026-04-27-ai-agent-eval-framework-design.md
```

### 2.2 数据流

```
用户触发 /analyze-library
        │
        ▼
读取库配置 (configs/<library>.yaml)
        │
        ▼
/discover-tests → 发现性能测试用例
        │
        ▼
遍历每个测试 → /inspect-function (定位被测函数)
        │
        ▼
/classify-difficulty (评估优化难度)
        │
        ▼
交互式确认 (用户确认/修改)
        │
        ▼
/generate-yaml → 生成 test_library/<library>.yaml
        │
        ▼
输出总结报告
```

### 2.3 组件设计

#### 2.3.1 Plugin 元数据

```json
{
  "name": "eval-framework",
  "version": "1.0.0",
  "description": "AI Agent 性能优化评估框架",
  "author": "eval-framework-team",
  "skills": [
    "analyze-library",
    "discover-tests",
    "inspect-function",
    "classify-difficulty",
    "generate-yaml"
  ]
}
```

#### 2.3.2 Skills 设计

**analyze-library** (主入口)
- 输入: 库名称或 `--all`
- 功能: 编排整个分析流程
- 输出: 分析报告和 YAML 文件

**discover-tests** (测试发现)
- 输入: 库路径、配置文件
- 功能: 扫描代码库，发现所有性能测试
- 方法: Glob 匹配 `*_perf.c`, `*_bench*.c` 等模式

**inspect-function** (函数检查)
- 输入: 测试文件路径
- 功能: 分析测试代码，定位被测函数
- 方法: 代码分析 + LSP 辅助

**classify-difficulty** (难度评估)
- 输入: 被测函数代码
- 功能: 评估优化难度
- 输出: easy/medium/hard + 理由

**generate-yaml** (YAML 生成)
- 输入: 所有已确认的用例数据
- 功能: 生成 YAML 文件
- 输出: test_library/<library>.yaml

## 3. 数据模型

### 3.1 库配置格式

```yaml
# configs/<library>.yaml
library:
  name: isa-l
  description: "Intel Intelligent Storage Acceleration Library"
  repo_path: /data/chenxuqiang/eval_framework/isa-l

build:
  type: autotools  # autotools | cmake | makefile
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
```

### 3.2 用例库格式

```yaml
# test_library/<library>.yaml
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
    difficulty: easy
    target_function:
      file: erasure_code/gf_multibinary.asm
      name: gf_vect_mul_avx512
      code_snippet: |
        ; AVX-512 implementation
        gf_vect_mul_avx512:
            endbranch64
            vmovdqa64 (zmm0, zmm1)
    analysis:
      reason: "Already uses AVX-512 vectorization"
      optimization_potential: "low"
      suggested_improvements: []
```

## 4. 工作流程

### 4.1 初始化阶段

1. 创建插件目录结构
2. 定义 Skills
3. 创建默认配置
4. 准备 YAML 模板

### 4.2 用例库构建阶段

1. 用户执行 `/analyze-library <library>`
2. Agent 加载库配置
3. Agent 发现所有性能测试
4. 对每个测试：
   - 分析被测函数
   - 评估优化难度
   - 用户交互确认
5. 生成 YAML 文件

### 4.3 评估阶段（后续实现）

1. AI Agent 读取用例库
2. 选择测试用例
3. 运行基线测试
4. 执行优化
5. 验证正确性
6. 测量性能提升
7. 生成评估报告

## 5. 难度分类标准

### 5.1 Easy（简单）

- 已经使用向量化指令（SIMD/NEON）
- 代码已经过良好优化
- 优化空间有限（< 10% 潜力提升）

### 5.2 Medium（中等）

- 纯 C/C++ 实现，可向量化
- 存在明显的优化机会（循环、内存访问）
- 需要重构但改动不大
- 预期 10-30% 性能提升

### 5.3 Hard（复杂）

- 算法级优化需求
- 复杂的内存访问模式
- 需要重新设计数据结构
- 涉及多线程并行化
- 预期 > 30% 性能提升

## 6. 错误处理

| 错误场景 | 处理方式 |
|---------|---------|
| 配置文件不存在 | 提示创建或使用默认配置 |
| 库无法构建 | 报告错误，跳过该库 |
| 无法定位被测函数 | 询问用户手动指定 |
| Agent 分析失败 | 提供重试或手动模式 |
| YAML 生成失败 | 显示原始数据供手动保存 |

## 7. 可扩展性

### 7.1 添加新库

1. 在 `configs/` 创建 `<library>.yaml`
2. 运行 `/analyze-library <library>`

### 7.2 添加新测试模式

修改配置中的 `perf_test_patterns`

### 7.3 自定义分类逻辑

修改 `/classify-difficulty` Skill

## 8. 使用示例

```bash
# 分析单个库
cd /data/chenxuqiang/eval_framework/isa-l
/analyze-library

# 分析所有库
/analyze-library --all

# 查看用例库
cat test_library/isa-l.yaml
```

## 9. 实现计划

### Phase 1: 插件基础设施
- [ ] 创建 plugin.json
- [ ] 创建目录结构
- [ ] 准备默认配置

### Phase 2: 核心 Skills
- [ ] 实现 discover-tests skill
- [ ] 实现 inspect-function skill
- [ ] 实现 classify-difficulty skill
- [ ] 实现 generate-yaml skill

### Phase 3: 主编排 Skill
- [ ] 实现 analyze-library skill
- [ ] 添加交互式确认流程
- [ ] 添加报告生成

### Phase 4: 用例库构建
- [ ] 分析 isa-l
- [ ] 分析 sleef
- [ ] 分析其他库

### Phase 5: 验证和完善
- [ ] 测试可扩展性
- [ ] 完善错误处理
- [ ] 文档完善
