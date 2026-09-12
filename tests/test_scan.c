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

static void test_result_is_unchanged_on_failure(void) {
    const int64_t values[] = {INT64_MAX, 1};
    forge_i64_scan_result result = {7, 99};

    assert(forge_i64_scan_ge(values, 2, INT64_MIN, &result) != 0);
    assert(result.count == 7);
    assert(result.sum == 99);
}

int main(void) {
    test_fused_scan();
    test_result_is_unchanged_on_failure();
    assert(forge_i64_scan_ge(NULL, 0, 0, &(forge_i64_scan_result){0, 0}) == 0);
    assert(forge_i64_scan_ge(NULL, 1, 0, &(forge_i64_scan_result){0, 0}) != 0);
    assert(forge_i64_scan_ge(NULL, 0, 0, NULL) != 0);
    return 0;
}
