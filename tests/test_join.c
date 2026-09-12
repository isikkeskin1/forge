#include "forge_join.h"

#include <assert.h>
#include <stdint.h>

static void test_inner_join(void) {
    const int64_t left[] = {10, 20, 10, 30};
    const int64_t right[] = {20, 10, 10, 40};

    forge_join_result result;
    forge_join_result_init(&result);

    assert(forge_i64_inner_join(left, 4, right, 4, &result) == 0);
    assert(result.length == 5);

    /* Every left row joins every matching right row. */
    assert(result.left[0] == 0 && result.right[0] == 2);
    assert(result.left[1] == 0 && result.right[1] == 1);
    assert(result.left[2] == 1 && result.right[2] == 0);
    assert(result.left[3] == 2 && result.right[3] == 2);
    assert(result.left[4] == 2 && result.right[4] == 1);

    forge_join_result_free(&result);
}

static void test_empty_and_reuse(void) {
    const int64_t value[] = {7};
    forge_join_result result;
    forge_join_result_init(&result);

    assert(forge_i64_inner_join(value, 1, value, 0, &result) == 0);
    assert(result.length == 0);
    assert(forge_i64_inner_join(value, 0, value, 1, &result) == 0);
    assert(result.length == 0);

    forge_join_result_free(&result);
}

static void test_invalid_arguments(void) {
    const int64_t value[] = {1};
    forge_join_result result;
    forge_join_result_init(&result);

    assert(forge_i64_inner_join(NULL, 1, value, 1, &result) != 0);
    assert(forge_i64_inner_join(value, 1, NULL, 1, &result) != 0);
    assert(forge_i64_inner_join(value, 1, value, 1, NULL) != 0);

    forge_join_result_free(&result);
}

int main(void) {
    test_inner_join();
    test_empty_and_reuse();
    test_invalid_arguments();
    return 0;
}
