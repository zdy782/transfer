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
