#include "forge_column.h"

#include <stdint.h>
#include <stdlib.h>

void forge_i64_column_init(forge_i64_column *column) {
    if (column == NULL) {
        return;
    }
    column->data = NULL;
    column->length = 0;
    column->capacity = 0;
}

void forge_i64_column_free(forge_i64_column *column) {
    if (column == NULL) {
        return;
    }
    free(column->data);
    forge_i64_column_init(column);
}

int forge_i64_column_reserve(forge_i64_column *column, size_t capacity) {
    if (column == NULL) {
        return -1;
    }
    if (capacity <= column->capacity) {
        return 0;
    }
    if (capacity > SIZE_MAX / sizeof(*column->data)) {
        return -1;
    }

    int64_t *data = realloc(column->data, capacity * sizeof(*column->data));
    if (data == NULL) {
        return -1;
    }
    column->data = data;
    column->capacity = capacity;
    return 0;
}

int forge_i64_column_append(forge_i64_column *column, int64_t value) {
    if (column == NULL) {
        return -1;
    }
    if (column->length == column->capacity) {
        size_t next = column->capacity == 0 ? 64 : column->capacity;
        if (next > SIZE_MAX / 2) {
            next = SIZE_MAX;
        } else {
            next *= 2;
        }
        if (next <= column->length || forge_i64_column_reserve(column, next) != 0) {
            return -1;
        }
    }

    column->data[column->length++] = value;
    return 0;
}

int64_t forge_i64_column_sum(const forge_i64_column *column) {
    if (column == NULL) {
        return 0;
    }

    int64_t sum = 0;
    for (size_t i = 0; i < column->length; ++i) {
        sum += column->data[i];
    }
    return sum;
}
