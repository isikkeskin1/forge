#include "forge_sort.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t forge_sort_key(int64_t value) {
    return (uint64_t)value ^ UINT64_C(0x8000000000000000);
}

static void forge_swap_i64(int64_t *a, int64_t *b) {
    const int64_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void forge_swap_size(size_t *a, size_t *b) {
    const size_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void forge_insertion_sort_i64(int64_t *data, size_t length) {
    for (size_t i = 1; i < length; ++i) {
        const int64_t value = data[i];
        size_t j = i;
        while (j > 0 && data[j - 1] > value) {
            data[j] = data[j - 1];
            --j;
        }
        data[j] = value;
    }
}

static void forge_insertion_sort_indices(const int64_t *data, size_t *indices,
                                         size_t length) {
    for (size_t i = 1; i < length; ++i) {
        const size_t index = indices[i];
        size_t j = i;
        while (j > 0) {
            const size_t previous = indices[j - 1];
            if (data[previous] < data[index] ||
                (data[previous] == data[index] && previous <= index)) {
                break;
            }
            indices[j] = previous;
            --j;
        }
        indices[j] = index;
    }
}

static void forge_sift_down(int64_t *data, size_t root, size_t end) {
    for (;;) {
        if (root > (end - 2) / 2) {
            return;
        }
        size_t child = root * 2 + 1;
        if (child + 1 < end && data[child] < data[child + 1]) {
            ++child;
        }
        if (data[root] >= data[child]) {
            return;
        }
        forge_swap_i64(&data[root], &data[child]);
        root = child;
    }
}

static void forge_sift_down_indices(const int64_t *data, size_t *indices,
                                    size_t root, size_t end) {
    for (;;) {
        if (root > (end - 2) / 2) {
            return;
        }
        size_t child = root * 2 + 1;
        const size_t left = indices[child];
        if (child + 1 < end) {
            const size_t right = indices[child + 1];
            if (data[left] < data[right] ||
                (data[left] == data[right] && left < right)) {
                ++child;
            }
        }
        const size_t current = indices[root];
        const size_t selected = indices[child];
        if (data[current] > data[selected] ||
            (data[current] == data[selected] && current >= selected)) {
            return;
        }
        forge_swap_size(&indices[root], &indices[child]);
        root = child;
    }
}

int forge_i64_sort(int64_t *data, size_t length) {
    if (data == NULL && length != 0) {
        return -1;
    }
    if (length < 2) {
        return 0;
    }

    /* Heap sort has O(n log n) worst-case behavior and needs no allocation. */
    for (size_t start = length / 2; start > 0; --start) {
        forge_sift_down(data, start - 1, length);
    }
    for (size_t end = length; end > 1; --end) {
        forge_swap_i64(&data[0], &data[end - 1]);
        forge_sift_down(data, 0, end - 1);
    }
    return 0;
}

int forge_i64_argsort(const int64_t *data, size_t length, size_t *indices) {
    if ((data == NULL || indices == NULL) && length != 0) {
        return -1;
    }
    if (length == 0) {
        return 0;
    }
    for (size_t i = 0; i < length; ++i) {
        indices[i] = i;
    }

    /* Stable tie-breaking by original row index makes permutations deterministic. */
    for (size_t start = length / 2; start > 0; --start) {
        forge_sift_down_indices(data, indices, start - 1, length);
    }
    for (size_t end = length; end > 1; --end) {
        forge_swap_size(&indices[0], &indices[end - 1]);
        forge_sift_down_indices(data, indices, 0, end - 1);
    }

    /* Heap sort above is deterministic; insertion pass repairs the tie ordering for
       small/equal runs while keeping the public permutation contract stable. */
    if (length <= 64) {
        forge_insertion_sort_indices(data, indices, length);
    }
    return 0;
}
