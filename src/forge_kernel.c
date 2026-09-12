#include "forge_kernel.h"

#include <limits.h>
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

int forge_i64_sum_ge(const int64_t *data, size_t length, int64_t threshold,
                     int64_t *sum) {
    if (sum == NULL || (data == NULL && length != 0)) {
        return -1;
    }

    int64_t result = 0;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] >= threshold) {
            const int64_t value = data[i];
            if ((value > 0 && result > INT64_MAX - value) ||
                (value < 0 && result < INT64_MIN - value)) {
                return -1;
            }
            result += value;
        }
    }

    *sum = result;
    return 0;
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
