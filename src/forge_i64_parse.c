#include "forge_i64_parse.h"

#include <stddef.h>
#include <stdint.h>

int forge_i64_parse(const char *text, int64_t *value) {
    if (text == NULL || value == NULL || *text == '\0') {
        return -1;
    }

    size_t index = 0;
    int negative = 0;
    if (text[index] == '+' || text[index] == '-') {
        negative = text[index] == '-';
        ++index;
    }
    if (text[index] == '\0') {
        return -1;
    }

    uint64_t magnitude = 0;
    const uint64_t limit = negative
        ? (uint64_t)INT64_MAX + 1u
        : (uint64_t)INT64_MAX;

    for (; text[index] != '\0'; ++index) {
        const unsigned char ch = (unsigned char)text[index];
        if (ch < '0' || ch > '9') {
            return -1;
        }

        const uint64_t digit = (uint64_t)(ch - '0');
        if (magnitude > (limit - digit) / 10u) {
            return -1;
        }
        magnitude = magnitude * 10u + digit;
    }

    if (negative) {
        if (magnitude == (uint64_t)INT64_MAX + 1u) {
            *value = INT64_MIN;
        } else {
            *value = -(int64_t)magnitude;
        }
    } else {
        *value = (int64_t)magnitude;
    }
    return 0;
}
