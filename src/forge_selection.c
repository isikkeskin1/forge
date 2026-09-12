#include "forge_selection.h"

#include <stdint.h>
#include <stdlib.h>

static int forge_size_mul_overflow(size_t a, size_t b) {
    return b != 0 && a > SIZE_MAX / b;
}

void forge_selection_init(forge_selection *selection) {
    if (selection == NULL) {
        return;
    }
    selection->indices = NULL;
    selection->length = 0;
    selection->capacity = 0;
}

void forge_selection_free(forge_selection *selection) {
    if (selection == NULL) {
        return;
    }
    free(selection->indices);
    forge_selection_init(selection);
}

int forge_selection_reserve(forge_selection *selection, size_t capacity) {
    if (selection == NULL) {
        return -1;
    }
    if (capacity <= selection->capacity) {
        return 0;
    }
    if (forge_size_mul_overflow(capacity, sizeof(*selection->indices))) {
        return -1;
    }

    size_t *indices = realloc(selection->indices,
                              capacity * sizeof(*selection->indices));
    if (indices == NULL) {
        return -1;
    }
    selection->indices = indices;
    selection->capacity = capacity;
    return 0;
}

int forge_selection_append(forge_selection *selection, size_t index) {
    if (selection == NULL) {
        return -1;
    }
    if (selection->length == selection->capacity) {
        size_t next = selection->capacity == 0 ? 64 : selection->capacity;
        if (next > SIZE_MAX / 2) {
            next = SIZE_MAX;
        } else {
            next *= 2;
        }
        if (next <= selection->length || forge_selection_reserve(selection, next) != 0) {
            return -1;
        }
    }
    selection->indices[selection->length++] = index;
    return 0;
}

static int forge_i64_select_predicate(const int64_t *data, size_t length,
                                      int64_t value, int predicate,
                                      forge_selection *selection) {
    if ((data == NULL && length != 0) || selection == NULL) {
        return -1;
    }

    selection->length = 0;
    for (size_t i = 0; i < length; ++i) {
        int match = 0;
        if (predicate == 0) {
            match = data[i] >= value;
        } else if (predicate == 1) {
            match = data[i] < value;
        } else {
            match = data[i] == value;
        }
        if (match && forge_selection_append(selection, i) != 0) {
            return -1;
        }
    }
    return 0;
}

int forge_i64_select_ge(const int64_t *data, size_t length,
                        int64_t threshold, forge_selection *selection) {
    return forge_i64_select_predicate(data, length, threshold, 0, selection);
}

int forge_i64_select_lt(const int64_t *data, size_t length,
                        int64_t threshold, forge_selection *selection) {
    return forge_i64_select_predicate(data, length, threshold, 1, selection);
}

int forge_i64_select_eq(const int64_t *data, size_t length,
                        int64_t value, forge_selection *selection) {
    return forge_i64_select_predicate(data, length, value, 2, selection);
}

int forge_selection_intersect(const forge_selection *left,
                             const forge_selection *right,
                             forge_selection *output) {
    if (left == NULL || right == NULL || output == NULL ||
        (left->length != 0 && left->indices == NULL) ||
        (right->length != 0 && right->indices == NULL)) {
        return -1;
    }

    output->length = 0;
    size_t left_pos = 0;
    size_t right_pos = 0;
    while (left_pos < left->length && right_pos < right->length) {
        const size_t left_index = left->indices[left_pos];
        const size_t right_index = right->indices[right_pos];
        if (left_index < right_index) {
            ++left_pos;
        } else if (right_index < left_index) {
            ++right_pos;
        } else {
            if (forge_selection_append(output, left_index) != 0) {
                return -1;
            }
            ++left_pos;
            ++right_pos;
        }
    }
    return 0;
}

size_t forge_i64_gather(const int64_t *data, size_t length,
                        const forge_selection *selection, int64_t *output) {
    if (selection == NULL || (data == NULL && selection->length != 0) ||
        (output == NULL && selection->length != 0)) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < selection->length; ++i) {
        const size_t index = selection->indices[i];
        if (index >= length) {
            return 0;
        }
        output[count++] = data[index];
    }
    return count;
}
