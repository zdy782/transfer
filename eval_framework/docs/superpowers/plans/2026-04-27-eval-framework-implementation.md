# AI Agent 性能优化评估框架 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a Claude Code plugin that automates ARM performance test case discovery and YAML generation for evaluating AI Agent optimization capabilities.

**Architecture:** Agent-driven orchestration — a main `analyze-library` Agent coordinates 4 Subagent Skills (discover-tests, inspect-function, classify-difficulty, generate-yaml). The Agent handles flow control, parallel scheduling, and user interaction; Skills handle individual analysis steps.

**Tech Stack:** Claude Code Plugin (plugin.json + agents + skills), YAML configs, Glob/Grep/LSP for code analysis

**Spec:** `docs/superpowers/specs/2026-04-27-ai-agent-eval-framework-design-v2.md`

---

## File Structure

| File | Responsibility |
|------|---------------|
| `.claude/plugins/eval-framework/.claude-plugin/plugin.json` | Plugin metadata |
| `.claude/plugins/eval-framework/agents/analyze-library.md` | Main orchestrating Agent |
| `.claude/plugins/eval-framework/skills/discover-tests/SKILL.md` | Discover perf test files |
| `.claude/plugins/eval-framework/skills/inspect-function/SKILL.md` | Analyze test code → locate target functions |
| `.claude/plugins/eval-framework/skills/classify-difficulty/SKILL.md` | Classify optimization difficulty |
| `.claude/plugins/eval-framework/skills/generate-yaml/SKILL.md` | Generate YAML test case library |
| `.claude/plugins/eval-framework/templates/test_case.yaml.tmpl` | Jinja2-like YAML template for test cases |
| `.claude/plugins/eval-framework/templates/library_config.yaml.tmpl` | Template for library config files |
| `configs/defaults.yaml` | Default configuration for all libraries |
| `configs/isa-l.yaml` | isa-l specific configuration |
| `configs/sleef.yaml` | sleef specific configuration |
| `test_library/` | Output directory for generated YAML files |

---

## Task 1: Create Plugin Skeleton

**Files:**
- Create: `.claude/plugins/eval-framework/.claude-plugin/plugin.json`
- Create: `configs/defaults.yaml`
- Create: `configs/isa-l.yaml`
- Create: `configs/sleef.yaml`
- Create: `test_library/.gitkeep`

- [ ] **Step 1: Create directory structure**

```bash
mkdir -p .claude/plugins/eval-framework/.claude-plugin
mkdir -p .claude/plugins/eval-framework/agents
mkdir -p .claude/plugins/eval-framework/skills/discover-tests
mkdir -p .claude/plugins/eval-framework/skills/inspect-function
mkdir -p .claude/plugins/eval-framework/skills/classify-difficulty
mkdir -p .claude/plugins/eval-framework/skills/generate-yaml
mkdir -p .claude/plugins/eval-framework/templates
mkdir -p configs
mkdir -p test_library
touch test_library/.gitkeep
```

- [ ] **Step 2: Create plugin.json**

```json
{
  "name": "eval-framework",
  "version": "1.0.0",
  "description": "AI Agent performance optimization evaluation framework for ARM platforms",
  "author": {
    "name": "eval-framework-team"
  },
  "keywords": ["evaluation", "arm", "performance", "optimization", "benchmark"]
}
```

Write to: `.claude/plugins/eval-framework/.claude-plugin/plugin.json`

- [ ] **Step 3: Create defaults.yaml**

```yaml
# Default configuration for all libraries
library:
  name: ""
  description: ""
  repo_path: ""

build:
  type: cmake  # autotools | cmake | meson | makefile
  commands:
    configure: ""
    build: ""
    test: ""

test_discovery:
  perf_test_patterns:
    - "*_perf.c"
    - "*_perf.cpp"
    - "*bench*.c"
    - "*bench*.cpp"
    - "*benchmark*"
  exclude_patterns:
    - "*/test/*"
    - "*/tests/*"
    - "*/.git/*"
  search_locations:
    - "."

benchmark_source: official  # official | third_party | mixed
third_party_benchmarks: []
```

Write to: `configs/defaults.yaml`

- [ ] **Step 4: Create isa-l.yaml config**

```yaml
library:
  name: isa-l
  description: "Intelligent Storage Acceleration Library"
  repo_path: /data/chenxuqiang/eval_framework/isa-l

build:
  type: autotools
  commands:
    configure: ./autogen.sh && ./configure
    build: make -j$(nproc)
    test: make check

test_discovery:
  perf_test_patterns:
    - "*_perf.c"
  exclude_patterns:
    - "*/test/*"
    - "*/tests/*"
  search_locations:
    - erasure_code/
    - crc/
    - raid/
    - igzip/
    - mem/

benchmark_source: official
```

Write to: `configs/isa-l.yaml`

- [ ] **Step 5: Create sleef.yaml config**

```yaml
library:
  name: sleef
  description: "SIMD Library for Evaluating Elementary Functions"
  repo_path: /data/chenxuqiang/eval_framework/sleef

build:
  type: cmake
  commands:
    configure: cmake -B build -DCMAKE_BUILD_TYPE=Release
    build: cmake --build build -j$(nproc)
    test: cmake --build build --target test

test_discovery:
  perf_test_patterns:
    - "*bench*.cpp"
    - "*bench*.c"
  exclude_patterns:
    - "*/test/*"
    - "*/tests/*"
  search_locations:
    - .

benchmark_source: official
```

Write to: `configs/sleef.yaml`

- [ ] **Step 6: Commit**

```bash
git add .claude/plugins/eval-framework/.claude-plugin/plugin.json \
        configs/defaults.yaml configs/isa-l.yaml configs/sleef.yaml \
        test_library/.gitkeep
git commit -m "feat: create eval-framework plugin skeleton with configs"
```

---

## Task 2: Create Templates

**Files:**
- Create: `.claude/plugins/eval-framework/templates/test_case.yaml.tmpl`
- Create: `.claude/plugins/eval-framework/templates/library_config.yaml.tmpl`

- [ ] **Step 1: Create test_case.yaml.tmpl**

The template uses simple `{{placeholder}}` markers that the generate-yaml Skill will replace.

```yaml
library:
  name: {{library_name}}
  description: "{{library_description}}"
  platform: arm64
  analyzed_at: "{{analyzed_at}}"
  version: "{{library_version}}"

metadata:
  total_test_cases: {{total_test_cases}}
  difficulty_distribution:
    easy: {{easy_count}}
    medium: {{medium_count}}
    hard: {{hard_count}}

test_cases:
{{test_cases_content}}
```

Each test case entry within `test_cases_content`:

```yaml
  - name: {{case_name}}
    perf_file: {{perf_file}}
    benchmark_source: {{benchmark_source}}
    difficulty: {{difficulty}}
    target_function:
      file: {{target_file}}
      name: {{target_function_name}}
      code_snippet: |
        {{code_snippet}}
      arm_features:
{{arm_features_list}}
    analysis:
      reason: "{{reason}}"
      optimization_potential: "{{optimization_potential}}"
      suggested_improvements:
{{suggested_improvements_list}}
```

Write to: `.claude/plugins/eval-framework/templates/test_case.yaml.tmpl`

- [ ] **Step 2: Create library_config.yaml.tmpl**

```yaml
library:
  name: {{library_name}}
  description: "{{library_description}}"
  repo_path: {{repo_path}}

build:
  type: {{build_type}}
  commands:
    configure: {{configure_command}}
    build: {{build_command}}
    test: {{test_command}}

test_discovery:
  perf_test_patterns:
{{perf_patterns_list}}
  exclude_patterns:
{{exclude_patterns_list}}
  search_locations:
{{search_locations_list}}

benchmark_source: {{benchmark_source}}
third_party_benchmarks: []
```

Write to: `.claude/plugins/eval-framework/templates/library_config.yaml.tmpl`

- [ ] **Step 3: Commit**

```bash
git add .claude/plugins/eval-framework/templates/
git commit -m "feat: add YAML templates for test cases and library configs"
```

---

## Task 3: Implement discover-tests Skill

**Files:**
- Create: `.claude/plugins/eval-framework/skills/discover-tests/SKILL.md`

- [ ] **Step 1: Write the SKILL.md**

```markdown
---
name: discover-tests
description: Use when needing to find performance test files in a code library — scans configured paths for benchmark and perf test patterns, supports both official and third-party benchmark sources
---

# Discover Performance Tests

## Purpose

Scan a code library to discover all performance/benchmark test files based on library configuration.

## Input

You will receive:
- `repo_path`: Absolute path to the library source code
- `perf_test_patterns`: Glob patterns to match perf test files (e.g., `*_perf.c`, `*bench*.c`)
- `exclude_patterns`: Glob patterns to exclude (e.g., `*/test/*`)
- `search_locations`: Subdirectories within repo_path to search
- `benchmark_source`: One of `official`, `third_party`, or `mixed`
- `third_party_benchmarks`: (optional) List of third-party benchmark configs with `name`, `path`, `build`

## Process

1. **Search official tests**: For each location in `search_locations`, use Glob to find files matching `perf_test_patterns` within `repo_path/<location>/`. Exclude files matching `exclude_patterns`.

2. **Count lines**: For each discovered file, use `wc -l` via Bash to get the line count. This helps assess test complexity.

3. **Handle third-party benchmarks**: If `benchmark_source` is `third_party` or `mixed`, also scan `third_party_benchmarks` entries. For each, find entry files (files containing benchmark functions or main() entry points).

4. **Deduplicate**: Remove duplicate entries if any pattern matches the same file multiple times.

## Output Format

Return results in this YAML structure:

```yaml
discovered_tests:
  official:
    - file: <relative_path_from_repo>
      abs_path: <absolute_path>
      size_lines: <int>
    - ...
  third_party:
    - name: <benchmark_name>
      path: <absolute_path>
      type: third_party
      entry_files:
        - <relative_path_from_benchmark_root>
    - ...
  summary:
    official_count: <int>
    third_party_count: <int>
    total_count: <int>
```

## Edge Cases

- If no files are found, report `official_count: 0` and suggest the user verify `perf_test_patterns` and `search_locations` in the config.
- If a `search_location` directory doesn't exist, skip it with a warning.
- If a third-party benchmark path is invalid, skip it with a warning.
```

Write to: `.claude/plugins/eval-framework/skills/discover-tests/SKILL.md`

- [ ] **Step 2: Manual verification — test with isa-l config**

Run a mental walkthrough: given the isa-l config (search_locations: `erasure_code/, crc/, raid/, igzip/, mem/`, pattern: `*_perf.c`), the skill should find all 23 perf files across those directories. Verify the Glob pattern logic is correct.

- [ ] **Step 3: Commit**

```bash
git add .claude/plugins/eval-framework/skills/discover-tests/SKILL.md
git commit -m "feat: implement discover-tests skill"
```

---

## Task 4: Implement inspect-function Skill

**Files:**
- Create: `.claude/plugins/eval-framework/skills/inspect-function/SKILL.md`

- [ ] **Step 1: Write the SKILL.md**

```markdown
---
name: inspect-function
description: Use when needing to analyze a performance test file to locate the target function being benchmarked — traces call chains and identifies ARM-specific code features
---

# Inspect Target Function

## Purpose

Analyze a performance test file to identify which function is actually being benchmarked, locate its source code, and detect ARM-specific features.

## Input

You will receive:
- `test_file`: Absolute path to the performance test file
- `repo_path`: Absolute path to the library root

## Process

1. **Read the test file**: Use Read to examine the test file content.

2. **Identify benchmarked function call**: Look for the core function call in the test's hot path. Common patterns:
   - Direct function calls in timing loops (e.g., `gf_vect_mul(...)`)
   - Function pointer dispatch tables (e.g., `func_ptr[variant](...)`)
   - Macro-expanded benchmark wrappers (e.g., `BENCHMARK(func)`)
   - API calls wrapping the actual computation

3. **Locate the function definition**:
   - First try LSP `goToDefinition` from the function call site
   - If LSP is unavailable or fails, use Grep to search for the function definition pattern (`func_name:` in assembly, `func_name(` in C/C++)
   - Check common ARM-specific paths: `aarch64/`, `arm/`, `neon/`, `sve/` subdirectories

4. **Read the target function source**: Use Read to get the function code. For assembly files (`.S`, `.asm`), read the full function body until the next label or `ret`/`bx lr`.

5. **Detect ARM features**: Scan the function code for:
   - **NEON**: `vld1q`, `vst1q`, `vmovq`, `vaddq`, `vmlaq`, `vextq`, NEON register names (`q0`-`q31`, `v0`-`v31`), `#include <arm_neon.h>`
   - **SVE**: `svld1`, `svst1`, `svwhilelt`, `svadd`, `svmla`, SVE predicate registers (`p0`-`p15`), `svbool_t`, `#include <arm_sve.h>`
   - **SVE2**: `svwhilerw`, `svwhilewr`, `svadalp`, SVE2-specific intrinsics
   - **ARM Crypto**: `sha1h`, `sha256h`, `aesd`, `aese`, `vmull.p64`
   - **Assembly**: File extension `.S` or `.asm`, or labels ending with `:`
   - **Conditional compilation**: `#ifdef __ARM_ARCH`, `#ifdef __aarch64__`, `#if defined(ARM_NEON)`

6. **Handle dispatch tables**: If the function uses a multi-variant dispatch (e.g., `gf_vect_mul_dispatch` selecting between AVX/NEON/SVE), identify ALL variant implementations and list them.

## Output Format

```yaml
target_functions:
  - file: <relative_path_from_repo>
    abs_path: <absolute_path>
    name: <function_name>
    code_snippet: |
      <function_source_code>
    arm_features:
      - <neon|sve|sve2|arm_crypto|assembly|conditional_arm>
    dispatch_variants:
      - name: <variant_name>
        file: <relative_path>
        arm_features: [...]
    # dispatch_variants only present if multi-dispatch pattern detected
```

If no target function can be identified:

```yaml
target_functions:
  - file: <test_file>
    name: unresolved
    reason: "<explanation>"
    arm_features: []
```

## Edge Cases

- Test file calls multiple functions: list all target functions found.
- Function is a macro or inline: trace to the actual implementation.
- Assembly file with multiple implementations: list all as dispatch_variants.
- Third-party benchmark (C++ with Google Benchmark): parse `BENCHMARK_REGISTER` and `BENCHMARK_DEFINE` macros.
```

Write to: `.claude/plugins/eval-framework/skills/inspect-function/SKILL.md`

- [ ] **Step 2: Commit**

```bash
git add .claude/plugins/eval-framework/skills/inspect-function/SKILL.md
git commit -m "feat: implement inspect-function skill"
```

---

## Task 5: Implement classify-difficulty Skill

**Files:**
- Create: `.claude/plugins/eval-framework/skills/classify-difficulty/SKILL.md`

- [ ] **Step 1: Write the SKILL.md**

```markdown
---
name: classify-difficulty
description: Use when needing to evaluate the optimization difficulty of a function — classifies as easy/medium/hard based on ARM optimization potential
---

# Classify Optimization Difficulty

## Purpose

Evaluate how difficult it would be for an AI Agent to optimize a function for ARM platforms, and classify the difficulty level.

## Input

You will receive:
- `function_code`: The source code of the target function
- `arm_features`: List of ARM features detected (e.g., `neon`, `sve`, `assembly`, `arm_crypto`)
- `function_name`: Name of the function
- `file_path`: Source file path

## Classification Criteria

### Easy

The function is already well-optimized for ARM:
- Already uses NEON/SVE intrinsics or assembly
- Code is vectorized and uses appropriate instruction sets
- Memory access patterns are already optimal
- Estimated optimization potential: **< 10%**

Indicators:
- Presence of `neon`, `sve`, `sve2`, `arm_crypto` in arm_features AND the code looks hand-optimized
- Assembly implementation with structured NEON/SVE instructions
- Already uses intrinsics with proper vector widths

### Medium

The function has clear optimization opportunities:
- Pure C/C++ scalar implementation that could be vectorized
- Contains loops over arrays that are amenable to SIMD
- Has suboptimal memory access patterns (strided access, pointer chasing)
- Needs restructuring but changes are localized
- Estimated optimization potential: **10-30%**

Indicators:
- No ARM features in arm_features (or only `conditional_arm` with fallback to scalar)
- Scalar loops with regular access patterns
- Missing prefetch or alignment optimizations
- Simple data dependencies that can be resolved with instruction scheduling

### Hard

The function requires significant algorithmic or structural changes:
- Algorithm-level optimization needed (different algorithm, math identity)
- Complex memory access patterns (indirect access, sparse data)
- Requires data structure redesign
- Needs multi-threaded parallelization
- Cross-function optimization required
- Estimated optimization potential: **> 30%**

Indicators:
- Complex control flow with many branches
- Pointer-heavy code with indirect array access
- Algorithmically bound (e.g., sorting, graph traversal)
- Tight coupling with other functions requiring co-optimization

## Process

1. **Review the code**: Read the function code carefully. Understand what it computes and how.

2. **Check ARM features**: If `arm_features` contains `neon`, `sve`, etc., check whether the usage is comprehensive or partial (e.g., only uses NEON for part of the computation).

3. **Identify bottlenecks**: Determine what limits performance:
   - Is it compute-bound? (heavy arithmetic → vectorization opportunity)
   - Is it memory-bound? (lots of loads/stores → prefetch/alignment opportunity)
   - Is it latency-bound? (data dependencies → instruction-level parallelism)
   - Is it algorithm-bound? (fundamental algorithm choice → need different algorithm)

4. **Classify**: Apply the criteria above. If uncertain between two levels, choose the higher one (more challenging) and add `low_confidence: true`.

5. **Suggest improvements**: Provide 1-3 specific, actionable optimization suggestions.

## Output Format

```yaml
classification:
  difficulty: <easy|medium|hard>
  low_confidence: <true|false>
  reason: "<1-2 sentence explanation>"
  optimization_potential: <low|medium|high>
  bottlenecks:
    - <compute|memory|latency|algorithm>
  suggested_improvements:
    - "<specific actionable suggestion>"
```

## Edge Cases

- If the function is very short (< 10 lines) and trivially vectorizable, still classify as medium if it's pure C (optimization IS needed, even if easy to do).
- If the function is already in NEON assembly but has an obvious SVE upgrade path, classify as easy with a note about the SVE opportunity.
- If arm_features is empty and the function is pure C with loops, lean toward medium.
```

Write to: `.claude/plugins/eval-framework/skills/classify-difficulty/SKILL.md`

- [ ] **Step 2: Commit**

```bash
git add .claude/plugins/eval-framework/skills/classify-difficulty/SKILL.md
git commit -m "feat: implement classify-difficulty skill"
```

---

## Task 6: Implement generate-yaml Skill

**Files:**
- Create: `.claude/plugins/eval-framework/skills/generate-yaml/SKILL.md`

- [ ] **Step 1: Write the SKILL.md**

```markdown
---
name: generate-yaml
description: Use when needing to generate a structured YAML test case library file from collected analysis results — creates the final output artifact
---

# Generate YAML Test Case Library

## Purpose

Take all confirmed test case analysis results and generate a structured YAML file following the test_library format.

## Input

You will receive:
- `library_name`: Library name
- `library_description`: Library description
- `library_version`: Version string (git branch or tag)
- `test_cases`: List of confirmed test case objects, each containing:
  - `name`: Test case name
  - `perf_file`: Path to the perf test file
  - `benchmark_source`: `official` or `third_party`
  - `difficulty`: `easy`, `medium`, or `hard`
  - `target_function`: Object with `file`, `name`, `code_snippet`, `arm_features`
  - `analysis`: Object with `reason`, `optimization_potential`, `suggested_improvements`

## Process

1. **Read template**: Use Read to load `.claude/plugins/eval-framework/templates/test_case.yaml.tmpl`.

2. **Calculate metadata**:
   - `total_test_cases`: Count of all test cases
   - `difficulty_distribution.easy`: Count of easy cases
   - `difficulty_distribution.medium`: Count of medium cases
   - `difficulty_distribution.hard`: Count of hard cases

3. **Format test cases**: For each test case, format the YAML entry following the template structure. Ensure:
   - `code_snippet` is properly indented in the YAML literal block
   - `arm_features` is a YAML list
   - `suggested_improvements` is a YAML list (empty list `[]` if none)
   - All string values are properly quoted

4. **Assemble the full YAML**: Combine metadata, library info, and formatted test cases into the final document.

5. **Validate**: Check the generated YAML is syntactically valid by verifying:
   - Proper indentation (2 spaces)
   - No trailing whitespace
   - All lists and dictionaries are properly closed
   - Code snippets in literal blocks are preserved

6. **Write output**: Write the YAML file to `test_library/<library_name>.yaml`.

## Output Format

Report the result:

```yaml
generation_result:
  status: success
  output_file: test_library/<library_name>.yaml
  total_cases: <int>
  difficulty_distribution:
    easy: <int>
    medium: <int>
    hard: <int>
```

If generation fails:

```yaml
generation_result:
  status: failed
  reason: "<error description>"
  raw_data_available: true
```

## Edge Cases

- If `suggested_improvements` is empty, output `[]` (not omitted).
- If `arm_features` is empty, output `[]`.
- If `code_snippet` is empty or unavailable, output `""` with a note.
- If a test case has `unresolved` target function, still include it with a warning flag.
```

Write to: `.claude/plugins/eval-framework/skills/generate-yaml/SKILL.md`

- [ ] **Step 2: Commit**

```bash
git add .claude/plugins/eval-framework/skills/generate-yaml/SKILL.md
git commit -m "feat: implement generate-yaml skill"
```

---

## Task 7: Implement analyze-library Agent

**Files:**
- Create: `.claude/plugins/eval-framework/agents/analyze-library.md`

- [ ] **Step 1: Write the Agent definition**

```markdown
---
name: analyze-library
description: |
  Use this agent when the user asks to "analyze a library", "generate test cases for a library", "build evaluation cases", "run eval framework", or wants to discover and classify ARM optimization test cases for a code library. Trigger when the user mentions performance evaluation, benchmark analysis, or test case generation for ARM optimization.

  <example>
  Context: User wants to generate evaluation test cases for isa-l
  user: "Analyze isa-l and generate test cases"
  assistant: "I'll use the analyze-library agent to discover perf tests, inspect functions, classify difficulty, and generate YAML."
  <commentary>
  User requesting library analysis for eval framework, trigger analyze-library agent.
  </commentary>
  </example>

  <example>
  Context: User wants to analyze all libraries
  user: "Run the eval framework on all libraries"
  assistant: "I'll use the analyze-library agent with --all flag to process every configured library."
  <commentary>
  User wants batch analysis of all libraries, trigger analyze-library agent.
  </commentary>
  </example>

  <example>
  Context: User wants to add a new library to the eval framework
  user: "I want to analyze sleef for ARM optimization potential"
  assistant: "I'll use the analyze-library agent to discover and classify sleef's perf tests."
  <commentary>
  User wants to evaluate a specific library's optimization potential, trigger analyze-library agent.
  </commentary>
  </example>
model: inherit
color: cyan
---

You are the main orchestrating Agent for the AI Agent performance optimization evaluation framework. You coordinate the analysis workflow by dispatching Subagent Skills for each step.

## Core Workflow

When invoked with `/analyze-library <library_name>` or `/analyze-library --all`:

### Step 1: Load Configuration

1. Read `configs/<library_name>.yaml`. If it doesn't exist, read `configs/defaults.yaml` and ask the user to provide:
   - `repo_path` (required)
   - `build.type` (required)
   - `search_locations` (optional, defaults to `["."]`)
   - `perf_test_patterns` (optional, defaults from defaults.yaml)

2. If `--all` is specified, iterate over all YAML files in `configs/` (excluding `defaults.yaml`).

### Step 2: Discover Tests

Dispatch the `eval-framework:discover-tests` Skill with the library configuration. Collect the list of discovered test files.

### Step 3: Inspect Functions

For each discovered test file, dispatch the `eval-framework:inspect-function` Skill. Multiple files can be inspected in parallel — use the Agent tool to dispatch multiple subagents simultaneously when files are independent.

Collect all `target_functions` results.

### Step 4: Classify Difficulty

For each target function, dispatch the `eval-framework:classify-difficulty` Skill. Multiple classifications can run in parallel.

Collect all classification results.

### Step 5: Interactive Confirmation

Present a summary table of all test cases:

```
| # | Test Case | Target Function | Difficulty | ARM Features | Potential |
|---|-----------|----------------|------------|-------------|-----------|
| 1 | ...       | ...            | medium     | none        | medium    |
| 2 | ...       | ...            | easy       | neon        | low       |
```

Ask the user to:
- Confirm all cases
- Remove cases they don't want
- Modify difficulty or other fields
- Add additional cases manually

### Step 6: Generate YAML

Dispatch the `eval-framework:generate-yaml` Skill with the confirmed test case data. The Skill will write to `test_library/<library_name>.yaml`.

### Step 7: Summary Report

Output a final report:

```
Analysis complete for <library_name>

Test cases: <N>
  Easy:   <count> (<pct>%)
  Medium: <count> (<pct>%)
  Hard:   <count> (<pct>%)

Output: test_library/<library_name>.yaml
```

## Parallel Dispatch Strategy

Steps 3 and 4 can be parallelized:
- In Step 3, dispatch multiple `inspect-function` subagents for different test files in a single Agent tool call
- In Step 4, dispatch multiple `classify-difficulty` subagents for different functions in a single Agent tool call
- Wait for all parallel subagents to complete before proceeding to the next step

## Error Handling

- If discover finds 0 tests: report and ask user to verify config
- If inspect fails for a file: mark as `unresolved`, continue with others
- If classify is uncertain: use `medium` with `low_confidence: true`
- If generate fails: output raw JSON data so user can manually save

## Important Notes

- Always confirm with the user before generating the final YAML (Step 5)
- Preserve the full code snippets — they are essential for the evaluation phase
- Track which step produced each piece of data for traceability
```

Write to: `.claude/plugins/eval-framework/agents/analyze-library.md`

- [ ] **Step 2: Commit**

```bash
git add .claude/plugins/eval-framework/agents/analyze-library.md
git commit -m "feat: implement analyze-library orchestrating agent"
```

---

## Task 8: End-to-End Validation with isa-l

**Files:**
- Verify: `.claude/plugins/eval-framework/` (all files)
- Verify: `configs/isa-l.yaml`
- Output: `test_library/isa-l.yaml` (generated)

- [ ] **Step 1: Verify plugin loads correctly**

Check that the plugin structure is valid by confirming:
1. `.claude-plugin/plugin.json` has the correct `name` field
2. All skill directories contain `SKILL.md` files
3. The agents directory contains `analyze-library.md`

```bash
find .claude/plugins/eval-framework -type f | sort
```

Expected: all files from Tasks 1-7 listed.

- [ ] **Step 2: Verify isa-l config is accurate**

Manually verify `configs/isa-l.yaml` search_locations and perf_test_patterns match the actual isa-l directory structure:

```bash
find /data/chenxuqiang/eval_framework/isa-l -name "*_perf.c" | head -25
```

Expected: ~23 perf files found across `erasure_code/`, `crc/`, `raid/`, `igzip/`.

- [ ] **Step 3: Run /analyze-library isa-l**

Execute the agent via Claude Code:

```
/analyze-library isa-l
```

Verify the workflow completes all 7 steps and produces `test_library/isa-l.yaml`.

- [ ] **Step 4: Validate output YAML**

Check `test_library/isa-l.yaml` for:
- Valid YAML syntax
- Correct metadata counts
- Non-empty code_snippet fields
- Reasonable difficulty distribution (expect more easy cases given isa-l's extensive ARM code)

- [ ] **Step 5: Commit generated output**

```bash
git add test_library/isa-l.yaml
git commit -m "feat: add isa-l test case library (auto-generated)"
```

---

## Task 9: Validation with sleef

**Files:**
- Verify: `configs/sleef.yaml`
- Output: `test_library/sleef.yaml` (generated)

- [ ] **Step 1: Verify sleef config is accurate**

```bash
find /data/chenxuqiang/eval_framework/sleef -name "*bench*" -o -name "*perf*" | head -10
```

Expected: 3 benchmark files found (dftbench.cpp, bench.cpp, benchmark.xhtml).

- [ ] **Step 2: Run /analyze-library sleef**

Execute the agent via Claude Code:

```
/analyze-library sleef
```

- [ ] **Step 3: Validate output YAML**

Check `test_library/sleef.yaml` for valid syntax and reasonable content.

- [ ] **Step 4: Commit generated output**

```bash
git add test_library/sleef.yaml
git commit -m "feat: add sleef test case library (auto-generated)"
```

---

## Self-Review

**1. Spec coverage check:**

| Spec Section | Task |
|---|---|
| Plugin metadata (plugin.json) | Task 1 |
| Directory structure | Task 1 |
| defaults.yaml + configs | Task 1 |
| Templates | Task 2 |
| discover-tests Skill | Task 3 |
| inspect-function Skill | Task 4 |
| classify-difficulty Skill | Task 5 |
| generate-yaml Skill | Task 6 |
| analyze-library Agent | Task 7 |
| End-to-end validation (isa-l) | Task 8 |
| Second library validation (sleef) | Task 9 |
| Third-party benchmark support | Tasks 3, 4 (embedded in skill definitions) |
| Error handling | Task 7 (embedded in agent) |
| YAML data model | Tasks 2, 6 |

**2. Placeholder scan:** No TBD/TODO/fill-in-later found. All code blocks contain actual content.

**3. Type consistency:** 
- `arm_features` is consistently a list across inspect-function output, classify-difficulty input, and test_case.yaml.tmpl
- `benchmark_source` enum (`official|third_party|mixed`) consistent across config format, discover-tests, and generate-yaml
- `difficulty` enum (`easy|medium|hard`) consistent across classify-difficulty and test_case format
- `optimization_potential` enum (`low|medium|high`) consistent across classify-difficulty and test_case format

**Note:** Skill main file is named `SKILL.md` (uppercase), correcting the earlier `skill.md` convention from the design doc.
