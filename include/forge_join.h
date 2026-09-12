#ifndef FORGE_JOIN_H
#define FORGE_JOIN_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t *left;
    size_t *right;
    size_t length;
    size_t capacity;
} forge_join_result;

void forge_join_result_init(forge_join_result *result);
void forge_join_result_free(forge_join_result *result);

/* Build an inner equi-join between two int64 key columns. */
int forge_i64_inner_join(const int64_t *left_keys, size_t left_length,
                         const int64_t *right_keys, size_t right_length,
                         forge_join_result *result);

#endif
