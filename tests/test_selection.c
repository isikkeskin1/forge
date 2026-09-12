#include "forge_selection.h"

#include <assert.h>
#include <stdint.h>

static void test_select_and_gather(void) {
    const int64_t values[] = {-7, 4, 9, -2, 12, 4};
    int64_t output[6] = {0};

    forge_selection selection;
    forge_selection_init(&selection);

    assert(forge_i64_select_ge(values, 6, 4, &selection) == 0);
    assert(selection.length == 4);
    assert(selection.indices[0] == 1);
    assert(selection.indices[1] == 2);
    assert(selection.indices[2] == 4);
    assert(selection.indices[3] == 5);

    assert(forge_i64_gather(values, 6, &selection, output) == 4);
    assert(output[0] == 4);
    assert(output[1] == 9);
    assert(output[2] == 12);
    assert(output[3] == 4);

    forge_selection_free(&selection);
}

static void test_predicates(void) {
    const int64_t values[] = {1, 2, 3, 2, 5};
    forge_selection selection;
    forge_selection_init(&selection);

    assert(forge_i64_select_lt(values, 5, 3, &selection) == 0);
    assert(selection.length == 3);
    assert(selection.indices[0] == 0);
    assert(selection.indices[1] == 1);
    assert(selection.indices[2] == 3);

    assert(forge_i64_select_eq(values, 5, 2, &selection) == 0);
    assert(selection.length == 2);
    assert(selection.indices[0] == 1);
    assert(selection.indices[1] == 3);

    forge_selection_free(&selection);
}

static void test_reuse_and_reserve(void) {
    const int64_t values[] = {1, 2, 3, 4};
    forge_selection selection;
    forge_selection_init(&selection);

    assert(forge_selection_reserve(&selection, 128) == 0);
    assert(selection.capacity == 128);
    assert(forge_i64_select_ge(values, 4, 3, &selection) == 0);
    assert(selection.length == 2);
    assert(selection.indices[0] == 2);
    assert(selection.indices[1] == 3);

    forge_selection_free(&selection);
    assert(selection.indices == NULL);
    assert(selection.length == 0);
    assert(selection.capacity == 0);
}

static void test_intersection(void) {
    forge_selection left;
    forge_selection right;
    forge_selection output;
    forge_selection_init(&left);
    forge_selection_init(&right);
    forge_selection_init(&output);

    assert(forge_selection_append(&left, 1) == 0);
    assert(forge_selection_append(&left, 3) == 0);
    assert(forge_selection_append(&left, 7) == 0);
    assert(forge_selection_append(&right, 2) == 0);
    assert(forge_selection_append(&right, 3) == 0);
    assert(forge_selection_append(&right, 7) == 0);
    assert(forge_selection_append(&right, 9) == 0);

    assert(forge_selection_intersect(&left, &right, &output) == 0);
    assert(output.length == 2);
    assert(output.indices[0] == 3);
    assert(output.indices[1] == 7);

    forge_selection_free(&left);
    forge_selection_free(&right);
    forge_selection_free(&output);
}

static void test_invalid_arguments(void) {
    const int64_t values[] = {1};
    forge_selection selection;
    forge_selection_init(&selection);

    assert(forge_i64_select_ge(NULL, 1, 0, &selection) != 0);
    assert(forge_i64_select_ge(values, 1, 0, NULL) != 0);
    assert(forge_selection_append(NULL, 0) != 0);
    assert(forge_selection_reserve(NULL, 1) != 0);
    assert(forge_i64_gather(values, 1, NULL, (int64_t *)values) == 0);
    assert(forge_selection_intersect(NULL, &selection, &selection) != 0);

    forge_selection_free(&selection);
}

static void test_invalid_selection_index(void) {
    const int64_t values[] = {11, 22};
    int64_t output[2] = {0};
    forge_selection selection;
    forge_selection_init(&selection);

    assert(forge_selection_append(&selection, 1) == 0);
    assert(forge_selection_append(&selection, 99) == 0);
    assert(forge_i64_gather(values, 2, &selection, output) == 0);

    forge_selection_free(&selection);
}

int main(void) {
    test_select_and_gather();
    test_predicates();
    test_reuse_and_reserve();
    test_intersection();
    test_invalid_arguments();
    test_invalid_selection_index();
    return 0;
}
