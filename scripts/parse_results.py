#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path


def parse_key(output: str, key: str) -> str:
    match = re.search(rf'^{re.escape(key)}=(.+)$', output, re.MULTILINE)
    return match.group(1).strip() if match else ''


def parse_compare(path: Path) -> dict[str, str]:
    text = path.read_text(encoding='utf-8', errors='replace')
    return {
        'before': parse_key(text, '使用前(秒)'),
        'after': parse_key(text, '使用后(秒)'),
        'speedup': parse_key(text, '加速比').rstrip('x'),
        'improvement': parse_key(text, '性能提升'),
        'correctness': parse_key(text, '正确性'),
        'remark': parse_key(text, '备注'),
        'raw': ' | '.join(line.strip() for line in text.splitlines() if line.strip())[-500:],
    }


def parse_goto(path: Path) -> dict[str, str]:
    if not path.exists():
        return {'time': '', 'mflops': '', 'bandwidth': '', 'raw': 'missing'}
    text = path.read_text(encoding='utf-8', errors='replace')
    time_match = re.search(r'([0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)s\]', text)
    mflops_match = re.search(r'([0-9]+(?:\.[0-9]+)?)\s*MFlops', text)
    bandwidth_match = re.search(r'([0-9]+(?:\.[0-9]+)?)\s*MB/s', text)
    return {
        'time': time_match.group(1) if time_match else '',
        'mflops': mflops_match.group(1) if mflops_match else '',
        'bandwidth': bandwidth_match.group(1) if bandwidth_match else '',
        'raw': ' | '.join(line.strip() for line in text.splitlines() if line.strip())[-500:],
    }


def speedup(base: str, current: str) -> str:
    try:
        b = float(base)
        c = float(current)
    except ValueError:
        return ''
    if c <= 0.0:
        return ''
    return f'{b / c:.6f}'


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument('--arch', required=True)
    parser.add_argument('--case', required=True)
    parser.add_argument('--compare-out', required=True, type=Path)
    parser.add_argument('--goto-out', required=True, type=Path)
    parser.add_argument('--header', action='store_true')
    args = parser.parse_args()

    fieldnames = [
        'arch', 'case', 'variant', 'time_seconds', 'speedup_vs_original',
        'mflops', 'bandwidth_mb_s', 'correctness', 'remark', 'raw_summary'
    ]
    writer = csv.DictWriter(sys.stdout, fieldnames=fieldnames)
    if args.header:
        writer.writeheader()

    compare = parse_compare(args.compare_out)
    before = compare['before']
    rows = [
        {
            'arch': args.arch,
            'case': args.case,
            'variant': 'original',
            'time_seconds': before,
            'speedup_vs_original': '1.000000' if before else '',
            'mflops': '',
            'bandwidth_mb_s': '',
            'correctness': compare['correctness'],
            'remark': compare['remark'],
            'raw_summary': compare['raw'],
        },
        {
            'arch': args.arch,
            'case': args.case,
            'variant': 'optimized',
            'time_seconds': compare['after'],
            'speedup_vs_original': compare['speedup'] or speedup(before, compare['after']),
            'mflops': '',
            'bandwidth_mb_s': '',
            'correctness': compare['correctness'],
            'remark': compare['remark'],
            'raw_summary': compare['raw'],
        },
    ]
    goto = parse_goto(args.goto_out)
    rows.append({
        'arch': args.arch,
        'case': args.case,
        'variant': 'goto',
        'time_seconds': goto['time'],
        'speedup_vs_original': speedup(before, goto['time']),
        'mflops': goto['mflops'],
        'bandwidth_mb_s': goto['bandwidth'],
        'correctness': 'not_checked_by_package' if goto['time'] else 'missing',
        'remark': 'external_goto_binary',
        'raw_summary': goto['raw'],
    })

    for row in rows:
        writer.writerow(row)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
