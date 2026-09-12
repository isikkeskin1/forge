#ifndef FORGE_KERNEL_H
#define FORGE_KERNEL_H

#include <stddef.h>
#include <stdint.h>

/* Count values satisfying a scalar predicate. */
size_t forge_i64_count_ge(const int64_t *data, size_t length, int64_t threshold);

/* Sum values satisfying a scalar predicate. */
int64_t forge_i64_sum_ge(const int64_t *data, size_t length, int64_t threshold);

/* Copy values satisfying a scalar predicate into a caller-owned output array. */
size_t forge_i64_filter_ge(const int64_t *data, size_t length,
                           int64_t threshold, int64_t *output);

#endif
