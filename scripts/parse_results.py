#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import math
import re
from pathlib import Path
from typing import Optional


DETAIL_COLUMNS = (
    ("arch", "架构", 5),
    ("case", "接口", 6),
    ("size", "规模", 18),
    ("base", "基线(s)", 10),
    ("auto", "自动(s)", 10),
    ("opt", "优化(s)", 10),
    ("goto", "GOTO(s)", 10),
    ("base_over_opt", "基/优", 8),
    ("auto_over_opt", "自/优", 8),
    ("goto_over_opt", "G/优", 8),
    ("goto_perf", "GOTO性能", 20),
    ("status", "状态", 12),
)

SUMMARY_COLUMNS = (
    ("arch", "架构", 5),
    ("case", "接口", 6),
    ("sizes", "规模数", 6),
    ("base_gm", "基/优GM", 10),
    ("auto_gm", "自/优GM", 10),
    ("goto_gm", "G/优GM", 10),
    ("base_win", "优胜基线", 10),
    ("auto_win", "优胜自动", 10),
    ("goto_win", "优胜GOTO", 10),
    ("best", "最佳规模", 18),
    ("status", "状态", 12),
)


def parse_key(output: str, key: str) -> str:
    match = re.search(rf"^{re.escape(key)}=(.+)$", output, re.MULTILINE)
    return match.group(1).strip() if match else ""


def parse_compare(path: Path) -> dict[str, str]:
    text = path.read_text(encoding="utf-8", errors="replace")
    return {
        "before": parse_key(text, "使用前(秒)"),
        "autovec": parse_key(text, "自动向量化(秒)"),
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
    value = ratio_value(numerator, denominator)
    return "-" if value is None else f"{value:.3f}x"


def ratio_value(numerator: str, denominator: str) -> Optional[float]:
    try:
        lhs = float(numerator)
        rhs = float(denominator)
    except ValueError:
        return None
    if rhs <= 0.0:
        return None
    return lhs / rhs


def parse_float(value: str) -> Optional[float]:
    if not value:
        return None
    try:
        return float(value)
    except ValueError:
        return None


def fmt_time(value: str) -> str:
    if not value:
        return "-"
    try:
        return f"{float(value):.3e}"
    except ValueError:
        return value


def goto_perf(goto: dict[str, str]) -> str:
    parts: list[str] = []
    if goto["mflops"]:
        parts.append(f"{float(goto['mflops']):.2f} MF")
    if goto["bandwidth"]:
        parts.append(f"{float(goto['bandwidth']):.2f} MB/s")
    return " / ".join(parts) if parts else "-"


def table_rule(
    left: str,
    fill: str,
    sep: str,
    right: str,
    columns: tuple[tuple[str, str, int], ...],
) -> str:
    body = sep.join(fill * width for _, _, width in columns)
    return f"{left}{body}{right}"


def table_header(columns: tuple[tuple[str, str, int], ...]) -> str:
    labels = "│".join(label.center(width) for _, label, width in columns)
    return "\n".join(
        (
            table_rule("╭", "─", "┬", "╮", columns),
            f"│{labels}│",
            table_rule("├", "─", "┼", "┤", columns),
        )
    )


def table_footer(columns: tuple[tuple[str, str, int], ...]) -> str:
    return table_rule("╰", "─", "┴", "╯", columns)


def table_row(row: dict[str, str], columns: tuple[tuple[str, str, int], ...]) -> str:
    cells = []
    for key, _, width in columns:
        value = row.get(key, "-")
        cells.append(value[:width].ljust(width))
    return f"│{'│'.join(cells)}│"


def build_record(args: argparse.Namespace) -> dict[str, object]:
    if args.compare_out is None or args.goto_out is None:
        raise SystemExit("--compare-out and --goto-out are required")
    compare = parse_compare(args.compare_out)
    goto = parse_goto(args.goto_out)
    status = compare["correctness"] or "-"
    if goto["ok"] != "1":
        status = f"{status}/GOTO缺失" if status != "-" else "GOTO缺失"

    base_over_opt = ratio_value(compare["before"], compare["after"])
    auto_over_opt = ratio_value(compare["autovec"], compare["after"])
    goto_over_opt = ratio_value(goto["time"], compare["after"])

    return {
        "arch": args.arch.upper(),
        "case": args.case,
        "size": args.size,
        "before": parse_float(compare["before"]),
        "autovec": parse_float(compare["autovec"]),
        "after": parse_float(compare["after"]),
        "goto": parse_float(goto["time"]),
        "base_over_opt": base_over_opt,
        "auto_over_opt": auto_over_opt,
        "goto_over_opt": goto_over_opt,
        "goto_ok": goto["ok"] == "1",
        "goto_perf": goto_perf(goto),
        "status": status,
    }


def detail_row(record: dict[str, object]) -> dict[str, str]:
    return {
        "arch": str(record["arch"]),
        "case": str(record["case"]),
        "size": str(record["size"]),
        "base": fmt_time("" if record["before"] is None else str(record["before"])),
        "auto": fmt_time("" if record["autovec"] is None else str(record["autovec"])),
        "opt": fmt_time("" if record["after"] is None else str(record["after"])),
        "goto": fmt_time("" if record["goto"] is None else str(record["goto"])),
        "base_over_opt": fmt_ratio(record["base_over_opt"]),
        "auto_over_opt": fmt_ratio(record["auto_over_opt"]),
        "goto_over_opt": fmt_ratio(record["goto_over_opt"]),
        "goto_perf": str(record["goto_perf"]),
        "status": str(record["status"]),
    }


def fmt_ratio(value: object) -> str:
    return "-" if value is None else f"{float(value):.3f}x"


def geomean(values: list[float]) -> Optional[float]:
    filtered = [value for value in values if value > 0.0 and math.isfinite(value)]
    if not filtered:
        return None
    return math.exp(sum(math.log(value) for value in filtered) / len(filtered))


def win_count(values: list[float]) -> str:
    if not values:
        return "-"
    wins = sum(1 for value in values if value > 1.0)
    return f"{wins}/{len(values)}"


def summary_rows(records_path: Path) -> list[dict[str, str]]:
    records = [
        json.loads(line)
        for line in records_path.read_text(encoding="utf-8").splitlines()
        if line.strip()
    ]
    rows: list[dict[str, str]] = []
    by_case: dict[str, list[dict[str, object]]] = {}
    for record in records:
        by_case.setdefault(str(record["case"]), []).append(record)

    for case_name in sorted(by_case):
        case_records = by_case[case_name]
        base_ratios = [
            float(record["base_over_opt"])
            for record in case_records
            if record.get("base_over_opt") is not None
        ]
        auto_ratios = [
            float(record["auto_over_opt"])
            for record in case_records
            if record.get("auto_over_opt") is not None
        ]
        goto_ratios = [
            float(record["goto_over_opt"])
            for record in case_records
            if record.get("goto_over_opt") is not None
        ]
        best_record = None
        if base_ratios:
            best_record = max(
                (
                    record
                    for record in case_records
                    if record.get("base_over_opt") is not None
                ),
                key=lambda record: float(record["base_over_opt"]),
            )

        status = "通过"
        if any(not str(record.get("status", "")).startswith("通过") for record in case_records):
            status = "有失败"
        elif not goto_ratios:
            status = "GOTO缺失"

        rows.append(
            {
                "arch": str(case_records[0]["arch"]),
                "case": case_name,
                "sizes": str(len(case_records)),
                "base_gm": fmt_ratio(geomean(base_ratios)),
                "auto_gm": fmt_ratio(geomean(auto_ratios)),
                "goto_gm": fmt_ratio(geomean(goto_ratios)),
                "base_win": win_count(base_ratios),
                "auto_win": win_count(auto_ratios),
                "goto_win": win_count(goto_ratios),
                "best": "-"
                if best_record is None
                else f"{best_record['size']} {fmt_ratio(best_record['base_over_opt'])}",
                "status": status,
            }
        )
    return rows


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--arch", default="")
    parser.add_argument("--case", default="")
    parser.add_argument("--size", default="-")
    parser.add_argument("--compare-out", type=Path)
    parser.add_argument("--goto-out", type=Path)
    parser.add_argument("--header", action="store_true")
    parser.add_argument("--footer", action="store_true")
    parser.add_argument("--record", action="store_true")
    parser.add_argument("--summary-records", type=Path)
    args = parser.parse_args()

    if args.header:
        print(table_header(DETAIL_COLUMNS))
        return 0
    if args.footer:
        print(table_footer(DETAIL_COLUMNS))
        return 0
    if args.summary_records is not None:
        print(table_header(SUMMARY_COLUMNS))
        for row in summary_rows(args.summary_records):
            print(table_row(row, SUMMARY_COLUMNS))
        print(table_footer(SUMMARY_COLUMNS))
        return 0

    record = build_record(args)
    if args.record:
        print(json.dumps(record, ensure_ascii=False, sort_keys=True))
        return 0

    print(table_row(detail_row(record), DETAIL_COLUMNS))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
