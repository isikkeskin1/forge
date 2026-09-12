#include "forge_group.h"

#include <assert.h>
#include <stdint.h>

static void test_group_add_and_get(void) {
    forge_i64_group group;
    forge_i64_group_init(&group);

    assert(forge_i64_group_add(&group, 10, 4) == 0);
    assert(forge_i64_group_add(&group, 10, 6) == 0);
    assert(forge_i64_group_add(&group, -3, 9) == 0);
    assert(forge_i64_group_add(&group, 10, -2) == 0);
    assert(group.size == 2);

    size_t count = 0;
    int64_t sum = 0;
    assert(forge_i64_group_get(&group, 10, &count, &sum) == 0);
    assert(count == 3);
    assert(sum == 8);
    assert(forge_i64_group_get(&group, -3, &count, &sum) == 0);
    assert(count == 1);
    assert(sum == 9);
    assert(forge_i64_group_get(&group, 99, &count, &sum) == 1);

    forge_i64_group_free(&group);
}

static void test_growth(void) {
    forge_i64_group group;
    forge_i64_group_init(&group);

    for (int64_t i = 0; i < 1000; ++i) {
        assert(forge_i64_group_add(&group, i, 1) == 0);
    }
    assert(group.size == 1000);
    assert(group.capacity >= group.size);

    for (int64_t i = 0; i < 1000; ++i) {
        size_t count = 0;
        assert(forge_i64_group_get(&group, i, &count, NULL) == 0);
        assert(count == 1);
    }
    forge_i64_group_free(&group);
}

static void test_scan(void) {
    const int64_t keys[] = {1, 2, 1, 3, 2, 1};
    const int64_t values[] = {10, 20, 5, 7, -2, 8};
    forge_i64_group group;
    forge_i64_group_init(&group);

    assert(forge_i64_group_scan(keys, values, 6, &group) == 0);

    size_t count = 0;
    int64_t sum = 0;
    assert(forge_i64_group_get(&group, 1, &count, &sum) == 0);
    assert(count == 3 && sum == 23);
    assert(forge_i64_group_get(&group, 2, &count, &sum) == 0);
    assert(count == 2 && sum == 18);
    assert(forge_i64_group_get(&group, 3, &count, &sum) == 0);
    assert(count == 1 && sum == 7);

    forge_i64_group_free(&group);
}

static void test_invalid_arguments(void) {
    forge_i64_group group;
    forge_i64_group_init(&group);

    assert(forge_i64_group_add(NULL, 1, 1) != 0);
    assert(forge_i64_group_reserve(NULL, 8) != 0);
    assert(forge_i64_group_get(NULL, 1, NULL, NULL) != 0);
    assert(forge_i64_group_get(&group, 1, NULL, NULL) != 0);
    assert(forge_i64_group_get(&group, 1, &(size_t){0}, NULL) != 0);
    assert(forge_i64_group_scan(NULL, NULL, 1, &group) != 0);
    assert(forge_i64_group_scan(NULL, NULL, 0, &group) == 0);
    assert(forge_i64_group_scan(NULL, (int64_t[]){1}, 1, &group) != 0);

    forge_i64_group_free(&group);
}

int main(void) {
    test_group_add_and_get();
    test_growth();
    test_scan();
    test_invalid_arguments();
    return 0;
}
