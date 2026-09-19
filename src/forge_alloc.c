#include "forge_alloc.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    size_t size;
} forge_alloc_header;

static forge_alloc_stats forge_stats;

static void forge_account_add(size_t size) {
    ++forge_stats.allocation_calls;
    ++forge_stats.live_allocations;
    forge_stats.live_bytes += size;
    forge_stats.total_requested_bytes += size;
    if (forge_stats.live_bytes > forge_stats.peak_live_bytes) {
        forge_stats.peak_live_bytes = forge_stats.live_bytes;
    }
}

static void forge_account_remove(size_t size) {
    ++forge_stats.free_calls;
    --forge_stats.live_allocations;
    forge_stats.live_bytes -= size;
}

void forge_alloc_reset_stats(void) {
    forge_stats = (forge_alloc_stats){0};
}

forge_alloc_stats forge_alloc_get_stats(void) {
    return forge_stats;
}

void *forge_malloc(size_t size) {
    if (size > SIZE_MAX - sizeof(forge_alloc_header)) {
        return NULL;
    }
    forge_alloc_header *header = malloc(sizeof(*header) + size);
    if (header == NULL) {
        return NULL;
    }
    header->size = size;
    forge_account_add(size);
    return header + 1;
}

void *forge_calloc(size_t count, size_t size) {
    if (size != 0 && count > SIZE_MAX / size) {
        return NULL;
    }
    const size_t bytes = count * size;
    void *ptr = forge_malloc(bytes);
    if (ptr != NULL && bytes != 0) {
        unsigned char *data = ptr;
        for (size_t i = 0; i < bytes; ++i) {
            data[i] = 0;
        }
    }
    return ptr;
}

void forge_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    forge_alloc_header *header = (forge_alloc_header *)ptr - 1;
    forge_account_remove(header->size);
    free(header);
}

void *forge_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return forge_malloc(size);
    }
    if (size > SIZE_MAX - sizeof(forge_alloc_header)) {
        return NULL;
    }

    forge_alloc_header *old_header = (forge_alloc_header *)ptr - 1;
    const size_t old_size = old_header->size;
    forge_alloc_header *new_header = realloc(old_header, sizeof(*new_header) + size);
    if (new_header == NULL) {
        return NULL;
    }
    new_header->size = size;

    ++forge_stats.allocation_calls;
    forge_stats.total_requested_bytes += size;
    if (size >= old_size) {
        forge_stats.live_bytes += size - old_size;
    } else {
        forge_stats.live_bytes -= old_size - size;
    }
    if (forge_stats.live_bytes > forge_stats.peak_live_bytes) {
        forge_stats.peak_live_bytes = forge_stats.live_bytes;
    }
    return new_header + 1;
}
