#include "forge_scan.h"

#include <assert.h>
#include <stdint.h>

static void test_fused_scan(void) {
    const int64_t values[] = {-5, 0, 10, 20, 20, 41};
    forge_i64_scan_result result = {0, 0};
    assert(forge_i64_scan_ge(values, 6, 20, &result) == 0);
    assert(result.count == 3);
    assert(result.sum == 81);
}

static void test_parallel_matches_scalar(void) {
    int64_t values[1003];
    for (size_t i = 0; i < 1003; ++i) {
        values[i] = (int64_t)(i % 97) - 40;
    }

    forge_i64_scan_result scalar = {0, 0};
    forge_i64_scan_result parallel = {0, 0};
    assert(forge_i64_scan_ge(values, 1003, 17, &scalar) == 0);

    forge_worker_pool *pool = forge_worker_pool_create(4);
    assert(pool != NULL);
    assert(forge_i64_scan_ge_parallel(pool, values, 1003, 17, 73, &parallel) == 0);
    assert(parallel.count == scalar.count);
    assert(parallel.sum == scalar.sum);

    forge_i64_scan_result empty = {9, 9};
    assert(forge_i64_scan_ge_parallel(pool, NULL, 0, 0, 64, &empty) == 0);
    assert(empty.count == 0 && empty.sum == 0);
    assert(forge_i64_scan_ge_parallel(pool, values, 1003, 17, 0, &parallel) != 0);
    forge_worker_pool_destroy(pool);
}

static void test_result_is_unchanged_on_failure(void) {
    const int64_t values[] = {INT64_MAX, 1};
    forge_i64_scan_result result = {7, 99};
    assert(forge_i64_scan_ge(values, 2, INT64_MIN, &result) != 0);
    assert(result.count == 7);
    assert(result.sum == 99);

    forge_worker_pool *pool = forge_worker_pool_create(2);
    assert(pool != NULL);
    assert(forge_i64_scan_ge_parallel(pool, values, 2, INT64_MIN, 1, &result) != 0);
    assert(result.count == 7);
    assert(result.sum == 99);
    forge_worker_pool_destroy(pool);
}

int main(void) {
    test_fused_scan();
    test_parallel_matches_scalar();
    test_result_is_unchanged_on_failure();
    assert(forge_i64_scan_ge(NULL, 0, 0, &(forge_i64_scan_result){0, 0}) == 0);
    assert(forge_i64_scan_ge(NULL, 1, 0, &(forge_i64_scan_result){0, 0}) != 0);
    assert(forge_i64_scan_ge(NULL, 0, 0, NULL) != 0);
    return 0;
}
