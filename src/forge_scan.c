#include "forge_scan.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

int forge_i64_scan_ge(const int64_t *data, size_t length, int64_t threshold,
                      forge_i64_scan_result *result) {
    if (result == NULL || (data == NULL && length != 0)) {
        return -1;
    }

    forge_i64_scan_result next = {0, 0};
    for (size_t i = 0; i < length; ++i) {
        if (data[i] < threshold) {
            continue;
        }

        if (next.count == SIZE_MAX) {
            return -1;
        }

        const int64_t value = data[i];
        if ((value > 0 && next.sum > INT64_MAX - value) ||
            (value < 0 && next.sum < INT64_MIN - value)) {
            return -1;
        }

        ++next.count;
        next.sum += value;
    }

    *result = next;
    return 0;
}
