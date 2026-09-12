#include "forge.h"

#include <stdio.h>

int main(void) {
    forge_u64_vector values;
    if (forge_u64_vector_init(&values, 8) != FORGE_OK) {
        fprintf(stderr, "failed to initialize vector\n");
        return 1;
    }

    for (uint64_t value = 1; value <= 10; ++value) {
        if (forge_u64_vector_push(&values, value) != FORGE_OK) {
            fprintf(stderr, "failed to append value\n");
            forge_u64_vector_free(&values);
            return 1;
        }
    }

    printf("Forge core smoke run: %zu values, sum=%llu\n",
           values.length,
           (unsigned long long)forge_u64_vector_sum(&values));

    forge_u64_vector_free(&values);
    return 0;
}
