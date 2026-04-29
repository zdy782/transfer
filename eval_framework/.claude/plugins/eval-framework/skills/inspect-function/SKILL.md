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
