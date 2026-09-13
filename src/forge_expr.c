#include "forge_expr.h"

#include <stdint.h>

static int forge_i64_add_checked(int64_t left, int64_t right, int64_t *out) {
    if (right > 0 && left > INT64_MAX - right) return -1;
    if (right < 0 && left < INT64_MIN - right) return -1;
    *out = left + right;
    return 0;
}

static int forge_i64_sub_checked(int64_t left, int64_t right, int64_t *out) {
    if (right > 0 && left < INT64_MIN + right) return -1;
    if (right < 0 && left > INT64_MAX + right) return -1;
    *out = left - right;
    return 0;
}

static int forge_i64_mul_checked(int64_t left, int64_t right, int64_t *out) {
    if (left == 0 || right == 0) { *out = 0; return 0; }
    if (left == -1 && right == INT64_MIN) return -1;
    if (right == -1 && left == INT64_MIN) return -1;
    if (left > 0) {
        if (right > 0) { if (left > INT64_MAX / right) return -1; }
        else { if (right < INT64_MIN / left) return -1; }
    } else {
        if (right > 0) { if (left < INT64_MIN / right) return -1; }
        else { if (left < INT64_MAX / right) return -1; }
    }
    *out = left * right;
    return 0;
}

static int forge_i64_binary(const int64_t *left, const int64_t *right,
                            size_t length, int64_t *output,
                            int (*op)(int64_t, int64_t, int64_t *)) {
    if ((left == NULL || right == NULL || output == NULL) && length != 0) return -1;
    for (size_t i = 0; i < length; ++i) {
        if (op(left[i], right[i], &output[i]) != 0) return -1;
    }
    return 0;
}

int forge_i64_add(const int64_t *left, const int64_t *right, size_t length, int64_t *output) {
    return forge_i64_binary(left, right, length, output, forge_i64_add_checked);
}

int forge_i64_sub(const int64_t *left, const int64_t *right, size_t length, int64_t *output) {
    return forge_i64_binary(left, right, length, output, forge_i64_sub_checked);
}

int forge_i64_mul(const int64_t *left, const int64_t *right, size_t length, int64_t *output) {
    return forge_i64_binary(left, right, length, output, forge_i64_mul_checked);
}

int forge_i64_add_selected(const int64_t *left, const int64_t *right,
                           const forge_selection *selection,
                           size_t length, int64_t *output) {
    if (selection == NULL || (left == NULL || right == NULL || output == NULL) && selection->length != 0) return -1;
    for (size_t i = 0; i < selection->length; ++i) {
        const size_t row = selection->indices[i];
        if (row >= length || forge_i64_add_checked(left[row], right[row], &output[i]) != 0) return -1;
    }
    return 0;
}
