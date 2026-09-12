#include "forge_group.h"

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
        if (*argv[1] == '\0' || end == argv[1] || *end != '\0' || parsed == 0 ||
            parsed > SIZE_MAX) {
            fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
            return 2;
        }
        length = (size_t)parsed;
    } else if (argc > 2) {
        fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
        return 2;
    }

    if (length > SIZE_MAX / (sizeof(int64_t) * 2)) {
        fprintf(stderr, "input size is too large\n");
        return 2;
    }

    int64_t *keys = malloc(length * sizeof(*keys));
    int64_t *values = malloc(length * sizeof(*values));
    if (keys == NULL || values == NULL) {
        free(keys);
        free(values);
        fprintf(stderr, "allocation failed for %zu rows\n", length);
        return 1;
    }

    for (size_t i = 0; i < length; ++i) {
        keys[i] = (int64_t)(i % 10000);
        values[i] = (int64_t)(i % 97) - 48;
    }

    forge_i64_group group;
    forge_i64_group_init(&group);

    const clock_t start = clock();
    const int status = forge_i64_group_scan(keys, values, length, &group);
    const clock_t end = clock();

    if (status != 0) {
        fprintf(stderr, "group-by failed\n");
        forge_i64_group_free(&group);
        free(keys);
        free(values);
        return 1;
    }

    size_t count = 0;
    int64_t sum = 0;
    if (forge_i64_group_get(&group, 42, &count, &sum) != 0) {
        fprintf(stderr, "verification lookup failed\n");
        forge_i64_group_free(&group);
        free(keys);
        free(values);
        return 1;
    }

    printf("rows: %zu\n", length);
    printf("distinct_keys: %zu\n", group.size);
    printf("probe_key: 42\n");
    printf("probe_count: %zu\n", count);
    printf("probe_sum: %" PRId64 "\n", sum);
    printf("cpu_time_seconds: %.6f\n", elapsed_seconds(start, end));

    forge_i64_group_free(&group);
    free(keys);
    free(values);
    return 0;
}
