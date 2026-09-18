#include "forge_kernel.h"

#include <assert.h>
#include <stdint.h>

static void test_scan_and_filter(void) {
    const int64_t values[] = {-10, 0, 4, 4, 9, 20};
    int64_t output[6] = {0};
    int64_t simd_output[6] = {0};
    int64_t sum = 0;

    assert(forge_i64_count_ge(values, 6, 4) == 4);
    assert(forge_i64_count_ge_simd(values, 6, 4) == 4);
    assert(forge_i64_sum_ge(values, 6, 4, &sum) == 0);
    assert(sum == 37);

    const size_t count = forge_i64_filter_ge(values, 6, 4, output);
    const size_t simd_count = forge_i64_filter_ge_simd(values, 6, 4, simd_output);
    assert(count == 4);
    assert(simd_count == count);
    for (size_t i = 0; i < count; ++i) {
        assert(simd_output[i] == output[i]);
    }
    assert(output[0] == 4);
    assert(output[1] == 4);
    assert(output[2] == 9);
    assert(output[3] == 20);
}

static void test_simd_matches_scalar(void) {
    int64_t values[257];
    int64_t scalar[257];
    int64_t vectorized[257];
    for (size_t i = 0; i < 257; ++i) {
        values[i] = (int64_t)((i * 37) % 101) - 50;
    }

    const int64_t thresholds[] = {INT64_MIN, -50, -1, 0, 17, 50, INT64_MAX};
    for (size_t i = 0; i < sizeof(thresholds) / sizeof(thresholds[0]); ++i) {
        const int64_t threshold = thresholds[i];
        assert(forge_i64_count_ge_simd(values, 257, threshold) ==
               forge_i64_count_ge(values, 257, threshold));

        const size_t scalar_count = forge_i64_filter_ge(values, 257, threshold, scalar);
        const size_t simd_count =
            forge_i64_filter_ge_simd(values, 257, threshold, vectorized);
        assert(simd_count == scalar_count);
        for (size_t row = 0; row < scalar_count; ++row) {
            assert(vectorized[row] == scalar[row]);
        }
    }

    assert(forge_i64_count_ge_simd(values, 3, 0) ==
           forge_i64_count_ge(values, 3, 0));
    assert(forge_i64_filter_ge_simd(values, 3, 0, vectorized) ==
           forge_i64_filter_ge(values, 3, 0, scalar));
    assert(forge_i64_count_ge_simd(NULL, 0, 0) == 0);
    assert(forge_i64_count_ge_simd(NULL, 1, 0) == 0);
    assert(forge_i64_filter_ge_simd(NULL, 0, 0, NULL) == 0);
    assert(forge_i64_filter_ge_simd(NULL, 1, 0, vectorized) == 0);
    assert(forge_i64_filter_ge_simd(values, 1, 0, NULL) == 0);
}

static void test_sum_overflow(void) {
    const int64_t values[] = {INT64_MAX, 1};
    int64_t sum = 0;
    assert(forge_i64_sum_ge(values, 2, INT64_MIN, &sum) != 0);

    const int64_t negative[] = {INT64_MIN, -1};
    assert(forge_i64_sum_ge(negative, 2, INT64_MIN, &sum) != 0);
}

static void test_invalid_arguments(void) {
    int64_t sum = 0;
    assert(forge_i64_count_ge(NULL, 0, 0) == 0);
    assert(forge_i64_sum_ge(NULL, 0, 0, &sum) == 0);
    assert(sum == 0);
    assert(forge_i64_sum_ge(NULL, 1, 0, &sum) != 0);
    assert(forge_i64_sum_ge(NULL, 0, 0, NULL) != 0);
    assert(forge_i64_filter_ge(NULL, 0, 0, NULL) == 0);
    assert(forge_i64_count_ge(NULL, 1, 0) == 0);
    assert(forge_i64_filter_ge((const int64_t[]){1}, 1, 0, NULL) == 0);
}

int main(void) {
    test_scan_and_filter();
    test_simd_matches_scalar();
    test_sum_overflow();
    test_invalid_arguments();
    return 0;
}
