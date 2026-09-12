# Forge microbenchmarks

Benchmarks are intentionally small and reproducible. They measure individual execution kernels before the query layer exists.

## Integer scan

Build `forge_bench_scan` with CMake and run it with an optional element count:

```text
forge_bench_scan
forge_bench_scan 50000000
```

The benchmark reports the input size, predicate threshold, match count, sum, and process CPU time. The default input contains 10 million `int64_t` values.

The result is deterministic, making it useful as a baseline when later SIMD, threading, and vectorized execution implementations are introduced.
