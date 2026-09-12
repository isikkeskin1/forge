#include "forge.h"

#include <assert.h>
#include <stdint.h>

int main(void) {
    forge_u64_vector vector;

    assert(forge_u64_vector_init(&vector, 0) == FORGE_OK);
    assert(vector.length == 0);
    assert(vector.capacity == 0);
    assert(vector.data == 0);

    for (uint64_t i = 1; i <= 1000; ++i) {
        assert(forge_u64_vector_push(&vector, i) == FORGE_OK);
    }

    assert(vector.length == 1000);
    assert(forge_u64_vector_sum(&vector) == 500500);

    forge_u64_vector_free(&vector);
    assert(vector.data == 0);
    assert(vector.length == 0);
    assert(vector.capacity == 0);

    return 0;
}
