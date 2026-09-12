# Forge

> A high-performance, embeddable analytical engine written in C.

This repository is the home of **Forge**, a focused systems/data-engineering project exploring how far a small, dependency-light C engine can push analytical workloads through columnar data, cache-aware execution, SIMD, parallelism, and memory-mapped I/O.

Forge is deliberately narrow: implement important pieces from first principles, benchmark them rigorously, and publish the engineering trade-offs. It is a laboratory for understanding high-performance data processing, not a replacement for DuckDB or Polars.

## Current status

Forge has a small analytical runtime with typed integer columns, strict numeric parsing, CSV ingestion, reusable selections, predicate and aggregation kernels, a typed table container, sorting/permutation primitives, hash joins, and a composable integer query pipeline. The pipeline keeps a row selection as its intermediate representation, allowing multiple filters to be chained before projection/materialization.

The project remains intentionally pre-optimization: correctness contracts and reproducible measurements come before SIMD or multithreading.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

To include microbenchmarks:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARKS=ON
cmake --build build
```

The pipeline benchmark accepts an optional row count:

```bash
./build/forge_bench_pipeline 5000000
```

## Roadmap

### Phase 1 — Core

- [x] CMake project
- [x] Public library API
- [x] Smoke tests
- [ ] Arena allocator
- [x] Typed column vectors
- [x] CSV scanner
- [x] Strict integer parser
- [x] CSV → integer column ingestion
- [x] Selection vectors
- [x] Typed table container
- [x] Benchmark scaffolding

### Phase 2 — Execution

- [x] Filter
- [x] Projection/materialization primitives
- [x] Fused scan + aggregation
- [x] Group-by hash table
- [x] Sort / argsort
- [x] Hash inner join
- [x] Query pipeline composition
- [ ] Expression kernels

### Phase 3 — Performance

- [ ] mmap reader
- [ ] pthread worker pool
- [ ] SIMD kernels
- [ ] Allocation profiling
- [ ] Cache and memory-bandwidth benchmarks
- [ ] Benchmark corpus and baseline comparisons

### Phase 4 — Interfaces

- [ ] SQL subset
- [ ] CLI query runner
- [ ] Python bindings
- [ ] Parquet reader

## Benchmark philosophy

Every optimization must be measured. Forge will publish workload definitions, hardware information, compiler flags, raw results, and correctness checks alongside headline numbers.

The goal is not to manufacture a benchmark victory. The goal is to understand **why** an implementation is faster or slower.

## Project structure

```text
include/    public headers
src/        engine implementation
tests/      correctness tests
bench/      reproducible performance benchmarks
docs/       architecture and experiment notes
```

## License

MIT
