#include "forge_table.h"

#include "forge_sort.h"

#include <stdint.h>
#include <stdlib.h>

void forge_table_init(forge_table *table) {
    if (table == NULL) {
        return;
    }
    table->columns = NULL;
    table->column_count = 0;
    table->row_count = 0;
}

void forge_table_free(forge_table *table) {
    if (table == NULL) {
        return;
    }
    if (table->columns != NULL) {
        for (size_t i = 0; i < table->column_count; ++i) {
            forge_i64_column_free(&table->columns[i]);
        }
    }
    free(table->columns);
    forge_table_init(table);
}

int forge_table_init_i64(forge_table *table, size_t column_count) {
    if (table == NULL) {
        return -1;
    }
    forge_table_free(table);
    if (column_count == 0 || column_count > SIZE_MAX / sizeof(*table->columns)) {
        return -1;
    }

    table->columns = calloc(column_count, sizeof(*table->columns));
    if (table->columns == NULL) {
        forge_table_init(table);
        return -1;
    }
    table->column_count = column_count;
    for (size_t i = 0; i < column_count; ++i) {
        forge_i64_column_init(&table->columns[i]);
    }
    return 0;
}

int forge_table_append_i64(forge_table *table, size_t column, int64_t value) {
    if (table == NULL || table->columns == NULL || column >= table->column_count) {
        return -1;
    }

    forge_i64_column *target = &table->columns[column];
    if (target->length != table->row_count) {
        return -1;
    }

    if (forge_i64_column_append(target, value) != 0) {
        return -1;
    }
    if (column + 1 == table->column_count) {
        table->row_count = target->length;
    }
    return 0;
}

int forge_table_validate(const forge_table *table) {
    if (table == NULL) {
        return -1;
    }
    if (table->column_count == 0) {
        return table->row_count == 0 ? 0 : -1;
    }
    if (table->columns == NULL) {
        return -1;
    }
    for (size_t i = 0; i < table->column_count; ++i) {
        if (table->columns[i].length != table->row_count) {
            return -1;
        }
    }
    return 0;
}

int forge_table_argsort_i64(const forge_table *table, size_t column, size_t *indices) {
    if (table == NULL || table->columns == NULL || column >= table->column_count) {
        return -1;
    }
    return forge_i64_argsort(table->columns[column].data,
                             table->row_count, indices);
}
