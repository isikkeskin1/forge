# Forge

> A high-performance, embeddable analytical engine written in C.

This repository is the home of **Forge**, a focused systems/data-engineering project exploring how far a small, dependency-light C engine can push analytical workloads through columnar data, cache-aware execution, SIMD, parallelism, and memory-mapped I/O.

Forge is deliberately narrow: implement important pieces from first principles, benchmark them rigorously, and publish the engineering trade-offs. It is a laboratory for understanding high-performance data processing, not a replacement for DuckDB or Polars.

## Current status

Forge has a small analytical runtime with typed integer columns, strict numeric parsing, CSV ingestion, reusable selections, predicate and aggregation kernels, checked integer expressions, a typed table container, sorting/permutation primitives, hash joins, and a composable integer query pipeline. Phase 3 includes read-only mapped files, a fixed-size pthread worker pool, partitioned parallel scan execution, AVX2 predicate/filter kernels with portable scalar fallbacks, and an allocation-accounting layer for measuring memory behavior.

The project remains intentionally correctness-first: optimized paths are introduced behind the same contracts as their scalar counterparts and are checked for result parity before broader vectorization.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

To compile the AVX2 path on a compatible x86-64 machine:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DFORGE_ENABLE_AVX2=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Without that option, the SIMD entry points remain portable and delegate to scalar implementations.

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
- [x] Expression kernels

### Phase 3 — Performance

- [x] mmap reader
- [x] pthread worker pool
- [x] Parallel scan execution
- [x] First SIMD predicate kernel
- [x] SIMD scan/filter specialization
- [x] Allocation profiling
- [ ] Cache and memory-bandwidth benchmarks
- [ ] Benchmark corpus and baseline comparisons

### Phase 4 — Interfaces

- [ ] SQL subset
- [ ] CLI query runner
- [ ] Python bindings
- [ ] Parquet reader

## Mapped I/O contract

`forge_mapped_file` owns a read-only mapping and its backing file descriptor. Initialize it with `forge_mapped_file_init`, open or replace a mapping with `forge_mapped_file_open`, and release it with `forge_mapped_file_close`. Empty files are valid mappings with a zero length and null data pointer. The current implementation uses POSIX `mmap`; Windows reports the operation as unsupported until a native mapping backend is added.

## Worker runtime and parallel scans

`forge_worker_pool` is a fixed-size pthread runtime with a bounded FIFO task queue. Producers block when the queue is full, workers sleep while it is empty, and `forge_worker_pool_wait` provides a completion barrier for submitted work. Pool destruction drains queued tasks before stopping and joining the worker threads.

`forge_i64_scan_ge_parallel` partitions a fused predicate/count/sum scan into configurable grain-sized chunks, schedules those chunks on an existing worker pool, and merges partial aggregates with the same overflow checks as the scalar scan. Callers retain control of the pool lifetime so repeated analytical operators can amortize thread creation.

## SIMD contract

`forge_i64_count_ge_simd` and `forge_i64_filter_ge_simd` vectorize the `>=` predicate on AVX2 builds. The filter keeps row order stable: fully matching vectors are stored directly, partially matching vectors are compacted lane-by-lane because AVX2 has no 64-bit compress-store, and arbitrary tails use the scalar path. Portable builds expose the same APIs and fall back to the scalar kernels so callers can keep one execution contract across targets.

## Allocation profiling

`forge_alloc` provides instrumented malloc/calloc/realloc/free wrappers and exposes counters for allocation/free calls, live allocations, current and peak live bytes, and cumulative requested bytes. The accounting layer is intentionally small and opt-in: operators can migrate to it incrementally, making allocation regressions measurable without coupling the analytical APIs to a particular allocator.

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
