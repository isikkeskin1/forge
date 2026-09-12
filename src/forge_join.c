#include "forge_join.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t forge_join_mix_u64(uint64_t value) {
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static size_t forge_join_next_power_of_two(size_t value) {
    if (value <= 1) {
        return 1;
    }
    --value;
    for (size_t shift = 1; shift < sizeof(value) * 8; shift <<= 1) {
        value |= value >> shift;
    }
    return value + 1;
}

static int forge_join_reserve_result(forge_join_result *result, size_t required) {
    if (required <= result->capacity) {
        return 0;
    }
    if (required > SIZE_MAX / sizeof(*result->left)) {
        return -1;
    }

    size_t capacity = result->capacity == 0 ? 64 : result->capacity;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2) {
            capacity = required;
            break;
        }
        capacity *= 2;
    }
    if (capacity > SIZE_MAX / sizeof(*result->left)) {
        return -1;
    }

    size_t *left = realloc(result->left, capacity * sizeof(*left));
    if (left == NULL) {
        return -1;
    }
    size_t *right = realloc(result->right, capacity * sizeof(*right));
    if (right == NULL) {
        /* Keep the original result valid; the left buffer may have moved. */
        result->left = left;
        return -1;
    }
    result->left = left;
    result->right = right;
    result->capacity = capacity;
    return 0;
}

void forge_join_result_init(forge_join_result *result) {
    if (result == NULL) {
        return;
    }
    result->left = NULL;
    result->right = NULL;
    result->length = 0;
    result->capacity = 0;
}

void forge_join_result_free(forge_join_result *result) {
    if (result == NULL) {
        return;
    }
    free(result->left);
    free(result->right);
    forge_join_result_init(result);
}

int forge_i64_inner_join(const int64_t *left_keys, size_t left_length,
                         const int64_t *right_keys, size_t right_length,
                         forge_join_result *result) {
    if (result == NULL || (left_keys == NULL && left_length != 0) ||
        (right_keys == NULL && right_length != 0)) {
        return -1;
    }
    if (left_length == 0 || right_length == 0) {
        result->length = 0;
        return 0;
    }
    if (right_length > (SIZE_MAX - 1) / 2) {
        return -1;
    }

    const size_t requested = right_length < 8 ? 8 : right_length * 2;
    const size_t bucket_count = forge_join_next_power_of_two(requested);
    if (bucket_count < requested) {
        return -1;
    }

    size_t *buckets = malloc(bucket_count * sizeof(*buckets));
    size_t *next = malloc(right_length * sizeof(*next));
    if (buckets == NULL || next == NULL) {
        free(buckets);
        free(next);
        return -1;
    }
    for (size_t i = 0; i < bucket_count; ++i) {
        buckets[i] = SIZE_MAX;
    }

    for (size_t i = 0; i < right_length; ++i) {
        const size_t bucket = (size_t)(forge_join_mix_u64((uint64_t)right_keys[i]) &
                                       (uint64_t)(bucket_count - 1));
        next[i] = buckets[bucket];
        buckets[bucket] = i;
    }

    result->length = 0;
    for (size_t left = 0; left < left_length; ++left) {
        const size_t bucket = (size_t)(forge_join_mix_u64((uint64_t)left_keys[left]) &
                                       (uint64_t)(bucket_count - 1));
        for (size_t right = buckets[bucket]; right != SIZE_MAX; right = next[right]) {
            if (right_keys[right] != left_keys[left]) {
                continue;
            }
            if (result->length == SIZE_MAX ||
                forge_join_reserve_result(result, result->length + 1) != 0) {
                free(buckets);
                free(next);
                return -1;
            }
            result->left[result->length] = left;
            result->right[result->length] = right;
            ++result->length;
        }
    }

    free(buckets);
    free(next);
    return 0;
}
