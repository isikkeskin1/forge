#ifndef FORGE_H
#define FORGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FORGE_OK = 0,
    FORGE_INVALID_ARGUMENT = 1,
    FORGE_OUT_OF_MEMORY = 2
} forge_status;

typedef struct {
    size_t length;
    size_t capacity;
    uint64_t *data;
} forge_u64_vector;

forge_status forge_u64_vector_init(forge_u64_vector *vector, size_t capacity);
void forge_u64_vector_free(forge_u64_vector *vector);
forge_status forge_u64_vector_push(forge_u64_vector *vector, uint64_t value);
uint64_t forge_u64_vector_sum(const forge_u64_vector *vector);

#ifdef __cplusplus
}
#endif

#endif
