#!/usr/bin/env python3
"""DuckDB adapter for Forge's external scan baseline contract.

Requires the optional `duckdb` Python package. Input construction is performed
outside the timed analytical query; only the filter/count/sum query is timed.
"""

import argparse
import csv
import sys
import time

try:
    import duckdb
except ImportError as exc:
    raise SystemExit("duckdb is required: python3 -m pip install duckdb") from exc


def run(rows: int, rounds: int, threshold: int) -> tuple[float, int, int, str]:
    connection = duckdb.connect(database=":memory:")
    version = connection.execute("select version()").fetchone()[0]

    # Build the deterministic corpus once. The expression matches Forge's
    # baseline generator: (row * 17) % 1009 - 504.
    connection.execute(
        "create table baseline as "
        "select ((i * 17) % 1009 - 504)::BIGINT as value "
        "from range(?) t(i)",
        [rows],
    )

    query = "select count(*), coalesce(sum(value), 0) from baseline where value >= ?"
    best = float("inf")
    expected = None

    for _ in range(rounds):
        start = time.perf_counter()
        count, total = connection.execute(query, [threshold]).fetchone()
        elapsed = time.perf_counter() - start
        best = min(best, elapsed)

        result = (int(count), int(total))
        if expected is None:
            expected = result
        elif result != expected:
            raise RuntimeError("DuckDB result changed between rounds")

    connection.close()
    assert expected is not None
    return best, expected[0], expected[1], version


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rows", type=int, default=10_000_000)
    parser.add_argument("--rounds", type=int, default=5)
    parser.add_argument("--threshold", type=int, default=0)
    args = parser.parse_args()
    if args.rows < 0 or args.rounds <= 0:
        parser.error("rows must be non-negative and rounds must be positive")

    elapsed, count, total, version = run(args.rows, args.rounds, args.threshold)
    throughput = args.rows / elapsed if elapsed else 0.0
    writer = csv.writer(sys.stdout)
    writer.writerow([
        "engine", "version", "rows", "rounds", "threshold", "seconds",
        "rows_per_second", "count", "sum"
    ])
    writer.writerow([
        "duckdb", version, args.rows, args.rounds, args.threshold,
        f"{elapsed:.9f}", f"{throughput:.3f}", count, total
    ])


if __name__ == "__main__":
    main()
