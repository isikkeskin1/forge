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

External-engine comparisons will be added only when Forge has equivalent semantics for the workload. Until then, this corpus is primarily for detecting regressions and quantifying internal optimization work.
