#include "forge_column.h"

#include <assert.h>
#include <stdint.h>

static void test_append_and_growth(void) {
    forge_i64_column column;
    forge_i64_column_init(&column);

    for (int64_t value = -100; value <= 100; ++value) {
        assert(forge_i64_column_append(&column, value) == 0);
    }

    assert(column.length == 201);
    assert(column.capacity >= column.length);
    assert(column.data[0] == -100);
    assert(column.data[200] == 100);
    assert(forge_i64_column_sum(&column) == 0);

    forge_i64_column_free(&column);
    assert(column.data == NULL);
    assert(column.length == 0);
    assert(column.capacity == 0);
}

static void test_reserve(void) {
    forge_i64_column column;
    forge_i64_column_init(&column);

    assert(forge_i64_column_reserve(&column, 128) == 0);
    assert(column.capacity == 128);
    assert(forge_i64_column_reserve(&column, 64) == 0);
    assert(column.capacity == 128);

    forge_i64_column_free(&column);
}

static void test_min_max(void) {
    forge_i64_column column;
    forge_i64_column_init(&column);

    const int64_t values[] = {42, -9, 17, -100, 64, 3};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        assert(forge_i64_column_append(&column, values[i]) == 0);
    }

    int64_t result = 0;
    assert(forge_i64_column_min(&column, &result) == 0);
    assert(result == -100);
    assert(forge_i64_column_max(&column, &result) == 0);
    assert(result == 64);

    forge_i64_column_free(&column);
}

static void test_empty_min_max(void) {
    forge_i64_column column;
    forge_i64_column_init(&column);
    int64_t result = 123;

    assert(forge_i64_column_min(&column, &result) != 0);
    assert(forge_i64_column_max(&column, &result) != 0);
    assert(forge_i64_column_min(NULL, &result) != 0);
    assert(forge_i64_column_max(&column, NULL) != 0);

    forge_i64_column_free(&column);
}

int main(void) {
    test_append_and_growth();
    test_reserve();
    test_min_max();
    test_empty_min_max();
    assert(forge_i64_column_append(NULL, 1) != 0);
    assert(forge_i64_column_reserve(NULL, 1) != 0);
    return 0;
}
