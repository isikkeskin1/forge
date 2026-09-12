#include "forge_i64_parse.h"

#include <assert.h>
#include <stdint.h>

static void test_valid_values(void) {
    int64_t value = 0;

    assert(forge_i64_parse("0", &value) == 0 && value == 0);
    assert(forge_i64_parse("42", &value) == 0 && value == 42);
    assert(forge_i64_parse("+17", &value) == 0 && value == 17);
    assert(forge_i64_parse("-99", &value) == 0 && value == -99);
    assert(forge_i64_parse("9223372036854775807", &value) == 0);
    assert(value == INT64_MAX);
    assert(forge_i64_parse("-9223372036854775808", &value) == 0);
    assert(value == INT64_MIN);
}

static void test_invalid_values(void) {
    int64_t value = 123;

    assert(forge_i64_parse("", &value) != 0);
    assert(forge_i64_parse("+", &value) != 0);
    assert(forge_i64_parse("-", &value) != 0);
    assert(forge_i64_parse("12x", &value) != 0);
    assert(forge_i64_parse("1.0", &value) != 0);
    assert(forge_i64_parse("9223372036854775808", &value) != 0);
    assert(forge_i64_parse("-9223372036854775809", &value) != 0);
    assert(forge_i64_parse(NULL, &value) != 0);
    assert(forge_i64_parse("1", NULL) != 0);
}

int main(void) {
    test_valid_values();
    test_invalid_values();
    return 0;
}
