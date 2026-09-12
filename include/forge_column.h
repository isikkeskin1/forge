#ifndef FORGE_COLUMN_H
#define FORGE_COLUMN_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int64_t *data;
    size_t length;
    size_t capacity;
} forge_i64_column;

void forge_i64_column_init(forge_i64_column *column);
void forge_i64_column_free(forge_i64_column *column);
int forge_i64_column_reserve(forge_i64_column *column, size_t capacity);
int forge_i64_column_append(forge_i64_column *column, int64_t value);
int64_t forge_i64_column_sum(const forge_i64_column *column);

#endif
