#include "forge_sort.h"

#include <stdint.h>
#include <stdlib.h>

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

static int forge_index_before(const int64_t *data, size_t left, size_t right) {
    return data[left] < data[right] ||
           (data[left] == data[right] && left < right);
}

static void forge_merge_indices(const int64_t *data, size_t *indices,
                                size_t *scratch, size_t begin, size_t middle,
                                size_t end) {
    size_t left = begin;
    size_t right = middle;
    size_t out = begin;

    while (left < middle && right < end) {
        if (forge_index_before(data, indices[left], indices[right]) ||
            indices[left] == indices[right]) {
            scratch[out++] = indices[left++];
        } else {
            scratch[out++] = indices[right++];
        }
    }
    while (left < middle) {
        scratch[out++] = indices[left++];
    }
    while (right < end) {
        scratch[out++] = indices[right++];
    }
    for (size_t i = begin; i < end; ++i) {
        indices[i] = scratch[i];
    }
}

static void forge_sort_indices_recursive(const int64_t *data, size_t *indices,
                                         size_t *scratch, size_t begin,
                                         size_t end) {
    if (end - begin < 2) {
        return;
    }
    const size_t middle = begin + (end - begin) / 2;
    forge_sort_indices_recursive(data, indices, scratch, begin, middle);
    forge_sort_indices_recursive(data, indices, scratch, middle, end);
    forge_merge_indices(data, indices, scratch, begin, middle, end);
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
    if (length < 2) {
        if (length == 1) {
            indices[0] = 0;
        }
        return 0;
    }
    if (length > SIZE_MAX / sizeof(*indices)) {
        return -1;
    }

    size_t *scratch = malloc(length * sizeof(*scratch));
    if (scratch == NULL) {
        return -1;
    }
    for (size_t i = 0; i < length; ++i) {
        indices[i] = i;
    }
    forge_sort_indices_recursive(data, indices, scratch, 0, length);
    free(scratch);
    return 0;
}
