#include "forge_kernel.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

int main(int argc, char **argv) {
    size_t length = 10000000;
    if (argc == 2) {
        char *end = NULL;
        const unsigned long long parsed = strtoull(argv[1], &end, 10);
        if (*argv[1] == '\0' || end == argv[1] || *end != '\0' || parsed == 0) {
            fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
            return 2;
        }
        length = (size_t)parsed;
    } else if (argc > 2) {
        fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
        return 2;
    }

    int64_t *data = malloc(length * sizeof(*data));
    if (data == NULL) {
        fprintf(stderr, "allocation failed for %zu elements\n", length);
        return 1;
    }

    for (size_t i = 0; i < length; ++i) {
        data[i] = (int64_t)(i % 1000);
    }

    const int64_t threshold = 500;
    const clock_t start = clock();
    const size_t count = forge_i64_count_ge(data, length, threshold);
    const int64_t sum = forge_i64_sum_ge(data, length, threshold);
    const clock_t end = clock();

    printf("elements: %zu\n", length);
    printf("threshold: %" PRId64 "\n", threshold);
    printf("matches: %zu\n", count);
    printf("sum: %" PRId64 "\n", sum);
    printf("cpu_time_seconds: %.6f\n", elapsed_seconds(start, end));

    free(data);
    return 0;
}
