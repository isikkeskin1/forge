#ifndef FORGE_TABLE_H
#define FORGE_TABLE_H

#include <stddef.h>

#include "forge_column.h"

typedef struct {
    forge_i64_column *columns;
    size_t column_count;
    size_t row_count;
} forge_table;

void forge_table_init(forge_table *table);
void forge_table_free(forge_table *table);
int forge_table_init_i64(forge_table *table, size_t column_count);
int forge_table_append_i64(forge_table *table, size_t column, long long value);
int forge_table_validate(const forge_table *table);

#endif
