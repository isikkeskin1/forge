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

static void test_invalid_arguments(void) {
    forge_i64_group group;
    forge_i64_group_init(&group);

    assert(forge_i64_group_add(NULL, 1, 1) != 0);
    assert(forge_i64_group_reserve(NULL, 8) != 0);
    assert(forge_i64_group_get(NULL, 1, NULL, NULL) != 0);
    assert(forge_i64_group_get(&group, 1, NULL, NULL) != 0);
    assert(forge_i64_group_get(&group, 1, &(size_t){0}, NULL) != 0);

    forge_i64_group_free(&group);
}

int main(void) {
    test_group_add_and_get();
    test_growth();
    test_invalid_arguments();
    return 0;
}
