#ifndef FORGE_SORT_H
#define FORGE_SORT_H

#include <stddef.h>
#include <stdint.h>

/* Sort an int64 column in ascending order in place. */
int forge_i64_sort(int64_t *data, size_t length);

/* Return a deterministic permutation that orders the input without modifying it. */
int forge_i64_argsort(const int64_t *data, size_t length, size_t *indices);

#endif
