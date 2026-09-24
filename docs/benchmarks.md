# Benchmark protocol

Forge benchmarks are intended to make performance changes reproducible rather than to produce isolated headline numbers.

## Baseline corpus

`forge_bench_baseline` is the stable starting workload. It currently measures the scalar fused `>=` scan over a deterministic integer column. The default corpus is 10,000,000 rows repeated five times. Output is CSV so results can be appended to experiment logs or compared by scripts without scraping prose.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARKS=ON
cmake --build build
./build/forge_bench_baseline
./build/forge_bench_baseline 10000000 10
```

The checksum is part of the output to keep the workload observable and to make accidental semantic changes visible alongside performance changes.

## Recording a baseline

For comparisons, record:

- Forge commit SHA
- compiler and version
- release flags and whether `FORGE_ENABLE_AVX2` is enabled
- CPU model and logical core count
- operating system
- workload row count and rounds
- at least three independent runs

Do not compare Debug builds with Release builds. Avoid running benchmarks while the machine is under significant unrelated load.

## Comparison policy

Use the same input shape and build configuration for before/after measurements. Report the individual runs as well as a median; do not discard slow runs without documenting why. Correctness checks take priority over a faster result.

## External-engine baseline contract

External comparisons start with the same fused integer scan as `forge_bench_baseline`: generate the deterministic integer sequence, retain values greater than or equal to the threshold, and report both the matching row count and sum. Adapters must validate those result fields before their timing is considered comparable.

`bench/external/baseline.py` is a dependency-free reference implementation of that contract. It is deliberately not presented as a competitive Python benchmark; its purpose is to make the input generator and result semantics executable outside Forge.

```bash
python3 bench/external/baseline.py --rows 1000000 --rounds 3 --threshold 0
```

### DuckDB adapter

`bench/external/duckdb_baseline.py` requires the optional `duckdb` Python package, creates the deterministic integer corpus in an in-memory DuckDB table before timing begins, then times only the `COUNT`/`SUM` query with the equivalent `>=` predicate. It validates that the result remains stable between rounds and includes the DuckDB version in its CSV output.

```bash
python3 -m pip install duckdb
python3 bench/external/duckdb_baseline.py --rows 10000000 --rounds 5 --threshold 0
```

### Polars adapter

`bench/external/polars_baseline.py` provides the same contract for Polars. It constructs the deterministic `Int64` column before timing, builds a lazy filter/aggregation expression, performs one untimed warm-up materialization, and then measures complete query execution for each round. The adapter verifies every measured result against the warm-up result and records the installed Polars version.

```bash
python3 -m pip install polars
python3 bench/external/polars_baseline.py --rows 10000000 --rounds 5 --threshold 0
```

The warm-up is intentional: lazy engines can incur one-time plan/runtime initialization that is not part of Forge's already-constructed scan benchmark. Measured rounds still execute the full filter and aggregation. Do not compare a cached/materialized Polars result against a fresh Forge scan.

Compare adapter output with a Release build of `forge_bench_baseline` using the same row count, round count, and threshold semantics. Package installation, process startup, and corpus construction are intentionally excluded from timed external queries; Forge's baseline similarly times the analytical scan rather than dataset construction.

An engine adapter belongs in `bench/external/` and should report engine/version, rows, rounds, threshold, seconds, rows_per_second, count, and sum. Time only the analytical operation after deterministic input construction when the engine API permits it. Do not include CSV parsing, package import, or process startup in one engine's measurement unless the same work is included for every engine.

A published comparison must include engine/version, Forge commit, compiler/build flags, hardware/OS, raw runs, and median throughput. Forge should not claim a performance win from the reference Python implementation; meaningful external claims require an adapter for the named engine and equivalent semantics. The repository provides the comparison harness, but measured performance claims should only be added after both engines have been run on the same machine under the protocol above.
