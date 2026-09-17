#ifndef FORGE_KERNEL_H
#define FORGE_KERNEL_H

#include <stddef.h>
#include <stdint.h>

/* Count values satisfying a scalar predicate. */
size_t forge_i64_count_ge(const int64_t *data, size_t length, int64_t threshold);

/* Use a compiled vector path when available, otherwise use the scalar kernel. */
size_t forge_i64_count_ge_simd(const int64_t *data, size_t length,
                               int64_t threshold);

/* Sum values satisfying a scalar predicate with explicit overflow reporting. */
int forge_i64_sum_ge(const int64_t *data, size_t length, int64_t threshold,
                     int64_t *sum);

/* Copy values satisfying a scalar predicate into a caller-owned output array. */
size_t forge_i64_filter_ge(const int64_t *data, size_t length,
                           int64_t threshold, int64_t *output);

#endif
