#!/usr/bin/env python3
"""Polars adapter for Forge's external scan baseline contract.

Requires the optional `polars` package. Corpus construction happens before the
timed expression; each round measures only filter plus count/sum aggregation.
"""

import argparse
import csv
import sys
import time

try:
    import polars as pl
except ImportError as exc:
    raise SystemExit("polars is required: python3 -m pip install polars") from exc


def build_corpus(rows: int) -> pl.DataFrame:
    # Match Forge's deterministic baseline generator exactly:
    # (row * 17) % 1009 - 504.
    return pl.DataFrame({"row": pl.arange(0, rows, eager=True, dtype=pl.Int64)}).select(
        (((pl.col("row") * 17) % 1009) - 504).alias("value")
    )


def run(rows: int, rounds: int, threshold: int) -> tuple[float, int, int, str]:
    frame = build_corpus(rows)
    query = frame.lazy().filter(pl.col("value") >= threshold).select(
        pl.len().alias("count"),
        pl.col("value").sum().alias("sum"),
    )

    # Materialize once before timing so lazy-plan initialization does not skew
    # the first measured round. The timed rounds still execute the full query.
    warmup = query.collect()
    expected = (int(warmup["count"][0]), int(warmup["sum"][0] or 0))
    best = float("inf")

    for _ in range(rounds):
        start = time.perf_counter()
        result = query.collect()
        elapsed = time.perf_counter() - start
        best = min(best, elapsed)

        observed = (int(result["count"][0]), int(result["sum"][0] or 0))
        if observed != expected:
            raise RuntimeError("Polars result changed between rounds")

    return best, expected[0], expected[1], pl.__version__


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
        "polars", version, args.rows, args.rounds, args.threshold,
        f"{elapsed:.9f}", f"{throughput:.3f}", count, total
    ])


if __name__ == "__main__":
    main()
