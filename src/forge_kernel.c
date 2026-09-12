#include "forge_kernel.h"

#include <stddef.h>
#include <stdint.h>

size_t forge_i64_count_ge(const int64_t *data, size_t length, int64_t threshold) {
    if (data == NULL && length != 0) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] >= threshold) {
            ++count;
        }
    }
    return count;
}

int64_t forge_i64_sum_ge(const int64_t *data, size_t length, int64_t threshold) {
    if (data == NULL && length != 0) {
        return 0;
    }

    int64_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] >= threshold) {
            sum += data[i];
        }
    }
    return sum;
}

size_t forge_i64_filter_ge(const int64_t *data, size_t length,
                           int64_t threshold, int64_t *output) {
    if ((data == NULL || output == NULL) && length != 0) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] >= threshold) {
            output[count++] = data[i];
        }
    }
    return count;
}
