#include "forge.h"

#include <stdlib.h>
#include <string.h>

static int forge_mul_overflow_size(size_t a, size_t b) {
    return b != 0 && a > SIZE_MAX / b;
}

forge_status forge_u64_vector_init(forge_u64_vector *vector, size_t capacity) {
    if (vector == NULL) {
        return FORGE_INVALID_ARGUMENT;
    }

    vector->length = 0;
    vector->capacity = capacity;
    vector->data = NULL;

    if (capacity == 0) {
        return FORGE_OK;
    }

    if (forge_mul_overflow_size(capacity, sizeof(*vector->data))) {
        return FORGE_OUT_OF_MEMORY;
    }

    vector->data = malloc(capacity * sizeof(*vector->data));
    if (vector->data == NULL) {
        vector->capacity = 0;
        return FORGE_OUT_OF_MEMORY;
    }

    return FORGE_OK;
}

void forge_u64_vector_free(forge_u64_vector *vector) {
    if (vector == NULL) {
        return;
    }

    free(vector->data);
    vector->data = NULL;
    vector->length = 0;
    vector->capacity = 0;
}

forge_status forge_u64_vector_push(forge_u64_vector *vector, uint64_t value) {
    if (vector == NULL) {
        return FORGE_INVALID_ARGUMENT;
    }

    if (vector->length == vector->capacity) {
        const size_t next_capacity = vector->capacity == 0 ? 16 : vector->capacity * 2;
        if (next_capacity < vector->capacity ||
            forge_mul_overflow_size(next_capacity, sizeof(*vector->data))) {
            return FORGE_OUT_OF_MEMORY;
        }

        uint64_t *next = realloc(vector->data, next_capacity * sizeof(*vector->data));
        if (next == NULL) {
            return FORGE_OUT_OF_MEMORY;
        }

        vector->data = next;
        vector->capacity = next_capacity;
    }

    vector->data[vector->length++] = value;
    return FORGE_OK;
}

uint64_t forge_u64_vector_sum(const forge_u64_vector *vector) {
    if (vector == NULL || vector->data == NULL) {
        return 0;
    }

    uint64_t sum = 0;
    for (size_t i = 0; i < vector->length; ++i) {
        sum += vector->data[i];
    }
    return sum;
}
