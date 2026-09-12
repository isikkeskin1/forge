#ifndef FORGE_SELECTION_H
#define FORGE_SELECTION_H

#include <stddef.h>
#include <stdint.h>

/* A compact list of row indices selected by a predicate. */
typedef struct {
    size_t *indices;
    size_t length;
    size_t capacity;
} forge_selection;

void forge_selection_init(forge_selection *selection);
void forge_selection_free(forge_selection *selection);
int forge_selection_reserve(forge_selection *selection, size_t capacity);
int forge_selection_append(forge_selection *selection, size_t index);

/* Build reusable selections from signed integer predicates. */
int forge_i64_select_ge(const int64_t *data, size_t length,
                        int64_t threshold, forge_selection *selection);
int forge_i64_select_lt(const int64_t *data, size_t length,
                        int64_t threshold, forge_selection *selection);
int forge_i64_select_eq(const int64_t *data, size_t length,
                        int64_t value, forge_selection *selection);

/* Intersect two sorted selections into a caller-owned selection. */
int forge_selection_intersect(const forge_selection *left,
                             const forge_selection *right,
                             forge_selection *output);

/* Materialize selected rows into a caller-owned integer array. */
size_t forge_i64_gather(const int64_t *data, size_t length,
                        const forge_selection *selection, int64_t *output);

#endif
