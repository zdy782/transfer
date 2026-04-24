#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
from pathlib import Path


COLUMNS = (
    ("arch", "架构", 6),
    ("case", "接口", 8),
    ("base", "基线(s)", 12),
    ("opt", "优化(s)", 12),
    ("goto", "GOTO(s)", 12),
    ("base_over_opt", "基线/优化", 12),
    ("goto_over_opt", "GOTO/优化", 12),
    ("goto_perf", "GOTO性能", 24),
    ("status", "状态", 14),
)


def parse_key(output: str, key: str) -> str:
    match = re.search(rf"^{re.escape(key)}=(.+)$", output, re.MULTILINE)
    return match.group(1).strip() if match else ""


def parse_compare(path: Path) -> dict[str, str]:
    text = path.read_text(encoding="utf-8", errors="replace")
    return {
        "before": parse_key(text, "使用前(秒)"),
        "after": parse_key(text, "使用后(秒)"),
        "correctness": parse_key(text, "正确性"),
        "remark": parse_key(text, "备注"),
    }


def parse_goto(path: Path) -> dict[str, str]:
    if not path.exists() or path.stat().st_size == 0:
        return {"time": "", "mflops": "", "bandwidth": "", "ok": "0"}
    text = path.read_text(encoding="utf-8", errors="replace")
    time_match = re.search(r"([0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)s\]", text)
    mflops_match = re.search(r"([0-9]+(?:\.[0-9]+)?)\s*MFlops", text)
    bandwidth_match = re.search(r"([0-9]+(?:\.[0-9]+)?)\s*MB/s", text)
    return {
        "time": time_match.group(1) if time_match else "",
        "mflops": mflops_match.group(1) if mflops_match else "",
        "bandwidth": bandwidth_match.group(1) if bandwidth_match else "",
        "ok": "1" if time_match else "0",
    }


def ratio(numerator: str, denominator: str) -> str:
    try:
        lhs = float(numerator)
        rhs = float(denominator)
    except ValueError:
        return "-"
    if rhs <= 0.0:
        return "-"
    return f"{lhs / rhs:.3f}x"


def fmt_time(value: str) -> str:
    if not value:
        return "-"
    try:
        return f"{float(value):.6g}"
    except ValueError:
        return value


def goto_perf(goto: dict[str, str]) -> str:
    parts: list[str] = []
    if goto["mflops"]:
        parts.append(f"{float(goto['mflops']):.2f} MF")
    if goto["bandwidth"]:
        parts.append(f"{float(goto['bandwidth']):.2f} MB/s")
    return " / ".join(parts) if parts else "-"


def table_rule(left: str, fill: str, sep: str, right: str) -> str:
    body = sep.join(fill * width for _, _, width in COLUMNS)
    return f"{left}{body}{right}"


def table_header() -> str:
    labels = "│".join(label.center(width) for _, label, width in COLUMNS)
    return "\n".join(
        (
            table_rule("╭", "─", "┬", "╮"),
            f"│{labels}│",
            table_rule("├", "─", "┼", "┤"),
        )
    )


def table_footer() -> str:
    return table_rule("╰", "─", "┴", "╯")


def table_row(row: dict[str, str]) -> str:
    cells = []
    for key, _, width in COLUMNS:
        value = row.get(key, "-")
        cells.append(value[:width].ljust(width))
    return f"│{'│'.join(cells)}│"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--arch", required=True)
    parser.add_argument("--case", required=True)
    parser.add_argument("--compare-out", required=True, type=Path)
    parser.add_argument("--goto-out", required=True, type=Path)
    parser.add_argument("--header", action="store_true")
    parser.add_argument("--footer", action="store_true")
    args = parser.parse_args()

    if args.header:
        print(table_header())
        return 0
    if args.footer:
        print(table_footer())
        return 0

    compare = parse_compare(args.compare_out)
    goto = parse_goto(args.goto_out)
    status = compare["correctness"] or "-"
    if goto["ok"] != "1":
        status = f"{status}/GOTO缺失" if status != "-" else "GOTO缺失"

    row = {
        "arch": args.arch.upper(),
        "case": args.case,
        "base": fmt_time(compare["before"]),
        "opt": fmt_time(compare["after"]),
        "goto": fmt_time(goto["time"]),
        "base_over_opt": ratio(compare["before"], compare["after"]),
        "goto_over_opt": ratio(goto["time"], compare["after"]),
        "goto_perf": goto_perf(goto),
        "status": status,
    }
    print(table_row(row))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
