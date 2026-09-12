#include "forge_join.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

int main(int argc, char **argv) {
    size_t length = 1000000;
    if (argc == 2) {
        char *end = NULL;
        const unsigned long long parsed = strtoull(argv[1], &end, 10);
        if (*argv[1] == '\0' || end == argv[1] || *end != '\0' || parsed == 0 ||
            (unsigned long long)(size_t)parsed != parsed ||
            (size_t)parsed > SIZE_MAX / sizeof(int64_t)) {
            fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
            return 2;
        }
        length = (size_t)parsed;
    } else if (argc > 2) {
        fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
        return 2;
    }

    int64_t *left = malloc(length * sizeof(*left));
    int64_t *right = malloc(length * sizeof(*right));
    if (left == NULL || right == NULL) {
        free(left);
        free(right);
        fprintf(stderr, "allocation failed for %zu elements per side\n", length);
        return 1;
    }

    for (size_t i = 0; i < length; ++i) {
        left[i] = (int64_t)(i % 100000);
        right[i] = (int64_t)((i * 3) % 100000);
    }

    forge_join_result result;
    forge_join_result_init(&result);

    const clock_t start = clock();
    const int status = forge_i64_inner_join(left, length, right, length, &result);
    const clock_t end = clock();
    if (status != 0) {
        forge_join_result_free(&result);
        free(left);
        free(right);
        fprintf(stderr, "join failed\n");
        return 1;
    }

    printf("left_elements: %zu\n", length);
    printf("right_elements: %zu\n", length);
    printf("matches: %zu\n", result.length);
    printf("cpu_time_seconds: %.6f\n", elapsed_seconds(start, end));
    if (result.length != 0) {
        printf("first_pair: %zu,%zu\n", result.left[0], result.right[0]);
        printf("last_pair: %zu,%zu\n",
               result.left[result.length - 1], result.right[result.length - 1]);
    }

    forge_join_result_free(&result);
    free(left);
    free(right);
    return 0;
}
