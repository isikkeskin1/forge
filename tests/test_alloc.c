#include "forge_alloc.h"

#include <assert.h>
#include <stdint.h>

static void test_malloc_and_free(void) {
    forge_alloc_reset_stats();
    void *a = forge_malloc(64);
    void *b = forge_calloc(4, 32);
    assert(a != NULL && b != NULL);

    forge_alloc_stats stats = forge_alloc_get_stats();
    assert(stats.allocation_calls == 2);
    assert(stats.live_allocations == 2);
    assert(stats.live_bytes == 192);
    assert(stats.peak_live_bytes == 192);
    assert(stats.total_requested_bytes == 192);

    forge_free(a);
    forge_free(b);
    stats = forge_alloc_get_stats();
    assert(stats.free_calls == 2);
    assert(stats.live_allocations == 0);
    assert(stats.live_bytes == 0);
    assert(stats.peak_live_bytes == 192);
}

static void test_realloc_accounting(void) {
    forge_alloc_reset_stats();
    void *ptr = forge_malloc(16);
    assert(ptr != NULL);
    ptr = forge_realloc(ptr, 128);
    assert(ptr != NULL);
    ptr = forge_realloc(ptr, 8);
    assert(ptr != NULL);

    forge_alloc_stats stats = forge_alloc_get_stats();
    assert(stats.allocation_calls == 3);
    assert(stats.live_allocations == 1);
    assert(stats.live_bytes == 8);
    assert(stats.peak_live_bytes == 128);
    assert(stats.total_requested_bytes == 152);

    forge_free(ptr);
    assert(forge_alloc_get_stats().live_bytes == 0);
}

static void test_calloc_zeroes_memory(void) {
    forge_alloc_reset_stats();
    uint64_t *values = forge_calloc(16, sizeof(*values));
    assert(values != NULL);
    for (size_t i = 0; i < 16; ++i) {
        assert(values[i] == 0);
    }
    forge_free(values);
}

static void test_overflow_is_rejected(void) {
    forge_alloc_reset_stats();
    assert(forge_calloc(SIZE_MAX, 2) == NULL);
    assert(forge_malloc(SIZE_MAX) == NULL);
    assert(forge_alloc_get_stats().allocation_calls == 0);
    forge_free(NULL);
}

int main(void) {
    test_malloc_and_free();
    test_realloc_accounting();
    test_calloc_zeroes_memory();
    test_overflow_is_rejected();
    return 0;
}
