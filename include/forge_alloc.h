#ifndef FORGE_ALLOC_H
#define FORGE_ALLOC_H

#include <stddef.h>

typedef struct {
    size_t allocation_calls;
    size_t free_calls;
    size_t live_allocations;
    size_t live_bytes;
    size_t peak_live_bytes;
    size_t total_requested_bytes;
} forge_alloc_stats;

void forge_alloc_reset_stats(void);
forge_alloc_stats forge_alloc_get_stats(void);

void *forge_malloc(size_t size);
void *forge_calloc(size_t count, size_t size);
void *forge_realloc(void *ptr, size_t size);
void forge_free(void *ptr);

#endif
