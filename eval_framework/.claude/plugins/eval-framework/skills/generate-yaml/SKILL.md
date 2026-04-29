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
