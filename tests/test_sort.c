#include "forge_sort.h"

#include <assert.h>
#include <stdint.h>

static void test_sort(void) {
    int64_t values[] = {9, -2, 4, 4, INT64_MIN, 17, -100, INT64_MAX, 0};
    const int64_t expected[] = {
        INT64_MIN, -100, -2, 0, 4, 4, 9, 17, INT64_MAX
    };

    assert(forge_i64_sort(values, sizeof(values) / sizeof(values[0])) == 0);
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        assert(values[i] == expected[i]);
    }
}

static void test_argsort(void) {
    const int64_t values[] = {30, 10, 20, 10, -5};
    const size_t expected[] = {4, 1, 3, 2, 0};
    size_t indices[5] = {0};

    assert(forge_i64_argsort(values, 5, indices) == 0);
    for (size_t i = 0; i < 5; ++i) {
        assert(indices[i] == expected[i]);
    }
}

static void test_empty_and_single(void) {
    int64_t value = 7;
    size_t index = 99;
    assert(forge_i64_sort(&value, 0) == 0);
    assert(forge_i64_sort(&value, 1) == 0);
    assert(value == 7);
    assert(forge_i64_argsort(&value, 1, &index) == 0);
    assert(index == 0);
    assert(forge_i64_argsort(NULL, 0, NULL) == 0);
}

static void test_invalid_arguments(void) {
    int64_t value = 1;
    size_t index = 0;
    assert(forge_i64_sort(NULL, 1) != 0);
    assert(forge_i64_argsort(NULL, 1, &index) != 0);
    assert(forge_i64_argsort(&value, 1, NULL) != 0);
}

int main(void) {
    test_sort();
    test_argsort();
    test_empty_and_single();
    test_invalid_arguments();
    return 0;
}
