#!/usr/bin/env python3
"""Reference implementation for Forge's external-engine scan baseline.

Uses only Python's standard library so the corpus can be validated without
installing a comparison engine. Optional engine adapters should preserve the
same deterministic input and checksum semantics.
"""

import argparse
import csv
import time


def value_at(row: int) -> int:
    """Match the deterministic integer distribution used by Forge baselines."""
    return (row * 17) % 1009 - 504


def run(rows: int, rounds: int, threshold: int) -> tuple[float, int, int]:
    best = float("inf")
    expected_count = 0
    expected_sum = 0

    for round_index in range(rounds):
        start = time.perf_counter()
        count = 0
        total = 0
        for row in range(rows):
            value = value_at(row)
            if value >= threshold:
                count += 1
                total += value
        elapsed = time.perf_counter() - start
        best = min(best, elapsed)

        if round_index == 0:
            expected_count = count
            expected_sum = total
        elif count != expected_count or total != expected_sum:
            raise RuntimeError("baseline result changed between rounds")

    return best, expected_count, expected_sum


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rows", type=int, default=10_000_000)
    parser.add_argument("--rounds", type=int, default=5)
    parser.add_argument("--threshold", type=int, default=0)
    args = parser.parse_args()
    if args.rows < 0 or args.rounds <= 0:
        parser.error("rows must be non-negative and rounds must be positive")

    elapsed, count, total = run(args.rows, args.rounds, args.threshold)
    writer = csv.writer(__import__("sys").stdout)
    writer.writerow(["engine", "rows", "rounds", "threshold", "seconds", "rows_per_second", "count", "sum"])
    throughput = args.rows / elapsed if elapsed else 0.0
    writer.writerow(["python", args.rows, args.rounds, args.threshold,
                     f"{elapsed:.9f}", f"{throughput:.3f}", count, total])


if __name__ == "__main__":
    main()
