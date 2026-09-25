#!/usr/bin/env python3
"""Validate and compare Forge/external benchmark CSV results.

The comparator consumes one or more CSV files produced by the benchmark corpus,
checks that they describe the same workload and result, then reports median
throughput relative to the first input. It intentionally does not execute
benchmarks so collection and comparison remain separate, auditable steps.
"""

from __future__ import annotations

import argparse
import csv
import statistics
import sys
from dataclasses import dataclass
from pathlib import Path


REQUIRED = {"engine", "rows", "rounds", "threshold", "seconds", "rows_per_second", "count", "sum"}


@dataclass(frozen=True)
class Run:
    engine: str
    rows: int
    threshold: int
    seconds: float
    rows_per_second: float
    count: int
    total: int


def read_runs(path: Path) -> list[Run]:
    with path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        missing = REQUIRED.difference(reader.fieldnames or ())
        if missing:
            raise ValueError(f"{path}: missing columns: {', '.join(sorted(missing))}")

        runs: list[Run] = []
        for line, row in enumerate(reader, start=2):
            try:
                run = Run(
                    engine=row["engine"].strip(),
                    rows=int(row["rows"]),
                    threshold=int(row["threshold"]),
                    seconds=float(row["seconds"]),
                    rows_per_second=float(row["rows_per_second"]),
                    count=int(row["count"]),
                    total=int(row["sum"]),
                )
            except (TypeError, ValueError) as exc:
                raise ValueError(f"{path}:{line}: invalid benchmark row: {exc}") from exc
            if not run.engine or run.rows < 0 or run.seconds <= 0 or run.rows_per_second <= 0:
                raise ValueError(f"{path}:{line}: invalid non-positive benchmark value")
            runs.append(run)

    if not runs:
        raise ValueError(f"{path}: no benchmark rows")
    return runs


def validate_comparable(groups: list[list[Run]]) -> None:
    reference = groups[0][0]
    expected = (reference.rows, reference.threshold, reference.count, reference.total)
    for runs in groups:
        engine = runs[0].engine
        for run in runs:
            if run.engine != engine:
                raise ValueError("each input file must contain exactly one engine")
            observed = (run.rows, run.threshold, run.count, run.total)
            if observed != expected:
                raise ValueError(
                    f"{engine}: workload/result mismatch; expected rows={expected[0]}, "
                    f"threshold={expected[1]}, count={expected[2]}, sum={expected[3]}"
                )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", nargs="+", type=Path, help="benchmark CSV files; first is the baseline")
    args = parser.parse_args()

    try:
        groups = [read_runs(path) for path in args.csv]
        validate_comparable(groups)
    except (OSError, ValueError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    baseline = statistics.median(run.rows_per_second for run in groups[0])
    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(("engine", "samples", "median_seconds", "median_rows_per_second", "relative_to_baseline"))
    for runs in groups:
        median_seconds = statistics.median(run.seconds for run in runs)
        median_throughput = statistics.median(run.rows_per_second for run in runs)
        writer.writerow((
            runs[0].engine,
            len(runs),
            f"{median_seconds:.9f}",
            f"{median_throughput:.3f}",
            f"{median_throughput / baseline:.6f}",
        ))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
