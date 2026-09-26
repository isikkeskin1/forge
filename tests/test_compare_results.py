#!/usr/bin/env python3
"""Regression tests for bench/external/compare_results.py."""

from __future__ import annotations

import csv
import importlib.util
import io
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "bench" / "external" / "compare_results.py"
spec = importlib.util.spec_from_file_location("compare_results", MODULE_PATH)
assert spec is not None and spec.loader is not None
compare_results = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compare_results)


HEADER = ["engine", "version", "rows", "rounds", "threshold", "seconds", "rows_per_second", "count", "sum"]


class CompareResultsTests(unittest.TestCase):
    def write_csv(self, rows: list[list[object]]) -> Path:
        handle = tempfile.NamedTemporaryFile(mode="w", newline="", suffix=".csv", delete=False)
        self.addCleanup(lambda: Path(handle.name).unlink(missing_ok=True))
        with handle:
            writer = csv.writer(handle)
            writer.writerow(HEADER)
            writer.writerows(rows)
        return Path(handle.name)

    def test_reads_valid_runs(self) -> None:
        path = self.write_csv([
            ["forge", "dev", 1000, 2, 0, 0.001, 1_000_000, 500, 125250],
            ["forge", "dev", 1000, 2, 0, 0.002, 500_000, 500, 125250],
        ])
        runs = compare_results.read_runs(path)
        self.assertEqual(len(runs), 2)
        self.assertEqual(runs[0].engine, "forge")
        self.assertEqual(runs[0].count, 500)
        self.assertEqual(runs[0].total, 125250)

    def test_rejects_missing_columns(self) -> None:
        path = self.write_csv([["forge", "dev", 1000, 1, 0, 0.001, 1_000_000, 500, 125250]])
        text = path.read_text(encoding="utf-8").replace(",sum\n", "\n")
        path.write_text(text, encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "missing columns"):
            compare_results.read_runs(path)

    def test_rejects_mixed_engines_in_one_file(self) -> None:
        path = self.write_csv([
            ["forge", "dev", 1000, 2, 0, 0.001, 1_000_000, 500, 125250],
            ["duckdb", "1.x", 1000, 2, 0, 0.001, 1_000_000, 500, 125250],
        ])
        with self.assertRaisesRegex(ValueError, "exactly one engine"):
            compare_results.validate_comparable([compare_results.read_runs(path)])

    def test_rejects_semantic_mismatch(self) -> None:
        forge = self.write_csv([["forge", "dev", 1000, 1, 0, 0.001, 1_000_000, 500, 125250]])
        duckdb = self.write_csv([["duckdb", "1.x", 1000, 1, 1, 0.001, 1_000_000, 499, 125250]])
        with self.assertRaisesRegex(ValueError, "workload/result mismatch"):
            compare_results.validate_comparable([
                compare_results.read_runs(forge),
                compare_results.read_runs(duckdb),
            ])

    def test_median_summary_is_stable(self) -> None:
        runs = [
            compare_results.Run("forge", 1000, 0, 0.004, 250_000, 500, 125250),
            compare_results.Run("forge", 1000, 0, 0.002, 500_000, 500, 125250),
            compare_results.Run("forge", 1000, 0, 0.003, 333_333, 500, 125250),
        ]
        self.assertEqual(compare_results.statistics.median(r.seconds for r in runs), 0.003)
        self.assertEqual(compare_results.statistics.median(r.rows_per_second for r in runs), 333_333)


if __name__ == "__main__":
    unittest.main()
