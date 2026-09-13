#include "forge_expr.h"

#include <assert.h>
#include <stdint.h>

static void test_arithmetic(void) {
    const int64_t a[] = {2, -4, 7};
    const int64_t b[] = {3, 6, -2};
    int64_t out[3];
    assert(forge_i64_add(a, b, 3, out) == 0);
    assert(out[0] == 5 && out[1] == 2 && out[2] == 5);
    assert(forge_i64_sub(a, b, 3, out) == 0);
    assert(out[0] == -1 && out[1] == -10 && out[2] == 9);
    assert(forge_i64_mul(a, b, 3, out) == 0);
    assert(out[0] == 6 && out[1] == -24 && out[2] == -14);
}

static void test_overflow(void) {
    const int64_t max = INT64_MAX;
    const int64_t min = INT64_MIN;
    int64_t out[1] = {123};
    assert(forge_i64_add(&max, &(int64_t){1}, 1, out) != 0);
    assert(out[0] == 123);
    assert(forge_i64_sub(&min, &(int64_t){1}, 1, out) != 0);
    assert(out[0] == 123);
    assert(forge_i64_mul(&min, &(int64_t){-1}, 1, out) != 0);
    assert(out[0] == 123);
}

static void test_selected(void) {
    const int64_t a[] = {10, 20, 30, 40};
    const int64_t b[] = {1, 2, 3, 4};
    forge_selection selection;
    forge_selection_init(&selection);
    assert(forge_selection_append(&selection, 3) == 0);
    assert(forge_selection_append(&selection, 1) == 0);
    int64_t out[2] = {0, 0};
    assert(forge_i64_add_selected(a, b, &selection, 4, out) == 0);
    assert(out[0] == 44 && out[1] == 22);
    forge_selection_free(&selection);
}

static void test_invalid_inputs(void) {
    assert(forge_i64_add(NULL, NULL, 0, NULL) == 0);
    assert(forge_i64_add(NULL, &(int64_t){1}, 1, &(int64_t){0}) != 0);
    forge_selection selection;
    forge_selection_init(&selection);
    assert(forge_i64_add_selected(NULL, NULL, &selection, 0, NULL) == 0);
    forge_selection_free(&selection);
}

int main(void) {
    test_arithmetic();
    test_overflow();
    test_selected();
    test_invalid_inputs();
    return 0;
}
