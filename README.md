# Forge

> A high-performance, embeddable analytical engine written in C.

This repository is the home of **Forge**, a focused systems/data-engineering project exploring how far a small, dependency-light C engine can push analytical workloads through columnar data, cache-aware execution, memory mapping, SIMD, and parallelism.

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

Early-stage foundation. The first milestone is a tiny, testable C core before adding query execution or optimizations.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## Roadmap

### Phase 1 — Core

- [x] CMake project
- [x] Public library API
- [x] Smoke tests
- [ ] Arena allocator
- [ ] Typed column vectors
- [ ] CSV parser
- [ ] Benchmark harness

### Phase 2 — Execution

- [ ] Filter
- [ ] Projection
- [ ] Aggregate
- [ ] Group-by hash table
- [ ] Sort
- [ ] Join

### Phase 3 — Performance

- [ ] mmap reader
- [ ] pthread worker pool
- [ ] SIMD kernels
- [ ] Allocation profiling
- [ ] Cache and memory-bandwidth benchmarks

### Phase 4 — Interfaces

- [ ] SQL subset
- [ ] CLI
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
