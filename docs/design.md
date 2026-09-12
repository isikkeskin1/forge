# Forge Architecture Notes

## 1. Execution model

Forge will use a column-oriented execution model. Operators should process contiguous arrays of the same type so scans can exploit cache locality and SIMD-friendly loops.

The first implementation intentionally avoids a large query planner. Operators will be explicit C functions with measurable contracts. A planner can be added once the execution primitives are stable.

## 2. Ownership

Every public object documents who owns its backing allocation. The core library will avoid hidden global state. Allocation failures are ordinary control-flow outcomes, not fatal process exits.

## 3. Correctness before optimization

Every optimized kernel must have a simple reference implementation. Benchmarks are only meaningful when the optimized result is checked against that reference.

Planned optimization ladder:

1. correct scalar implementation
2. benchmark baseline
3. reduce allocations/copies
4. improve memory locality
5. parallelize where profitable
6. add SIMD specialization
7. benchmark again

## 4. Benchmark contract

Benchmarks will record:

- compiler and version
- optimization flags
- CPU model
- input size and schema
- operation performed
- wall-clock time
- throughput
- peak memory

Forge will publish raw results rather than only reporting the fastest run.
