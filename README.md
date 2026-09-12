# Forge

> A high-performance, embeddable analytical engine written in C.

This repository is the home of **Forge**, a focused systems/data-engineering project exploring how far a small, dependency-light C engine can push analytical workloads through columnar data, cache-aware execution, SIMD, parallelism, and memory-mapped I/O.

## Why Forge?

Modern analytical engines are extremely capable, but they are also large systems. Forge is deliberately narrower: implement important pieces from first principles, benchmark them rigorously, and publish the engineering trade-offs.

Forge is **not** intended to replace DuckDB or Polars. It is a laboratory for understanding high-performance data processing.

## Design goals

- C11 core with explicit memory ownership
- Columnar execution for analytical workloads
- Zero-copy paths where practical
- Cache-aware algorithms and predictable memory access
- Parallel execution with a small worker runtime
- SIMD acceleration for hot loops
- Memory-mapped I/O for large local datasets
- Python bindings after the native core is stable
- Reproducible benchmarks against established tools

## Current status

Forge now has the first vertical slice of an analytical runtime: typed integer columns, strict numeric parsing, CSV ingestion, reusable selection vectors, scalar predicate kernels, fused scans, a columnar table container, and an open-addressed group-by hash table.

The project is still intentionally pre-optimization: correctness contracts and reproducible measurements come before SIMD or multithreading.

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
- [ ] Sort
- [ ] Join
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
