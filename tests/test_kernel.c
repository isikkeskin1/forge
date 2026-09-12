#include "forge_kernel.h"

#include <assert.h>
#include <stdint.h>

int main(void) {
    const int64_t values[] = {-10, 0, 4, 4, 9, 20};
    int64_t output[6] = {0};

    assert(forge_i64_count_ge(values, 6, 4) == 4);
    assert(forge_i64_sum_ge(values, 6, 4) == 37);

    const size_t count = forge_i64_filter_ge(values, 6, 4, output);
    assert(count == 4);
    assert(output[0] == 4);
    assert(output[1] == 4);
    assert(output[2] == 9);
    assert(output[3] == 20);

    assert(forge_i64_count_ge(NULL, 0, 0) == 0);
    assert(forge_i64_sum_ge(NULL, 0, 0) == 0);
    assert(forge_i64_filter_ge(NULL, 0, 0, NULL) == 0);
    assert(forge_i64_count_ge(NULL, 1, 0) == 0);
    assert(forge_i64_filter_ge(values, 1, 0, NULL) == 0);

    return 0;
}
