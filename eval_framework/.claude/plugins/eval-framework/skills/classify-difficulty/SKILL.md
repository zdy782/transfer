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
