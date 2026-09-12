#include "forge_sort.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

static int parse_length(const char *text, size_t *length) {
    char *end = NULL;
    const unsigned long long parsed = strtoull(text, &end, 10);
    if (*text == '\0' || end == text || *end != '\0' || parsed == 0 ||
        (unsigned long long)(size_t)parsed != parsed ||
        (size_t)parsed > SIZE_MAX / sizeof(int64_t)) {
        return -1;
    }
    *length = (size_t)parsed;
    return 0;
}

int main(int argc, char **argv) {
    size_t length = 1000000;
    if (argc == 2) {
        if (parse_length(argv[1], &length) != 0) {
            fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
            return 2;
        }
    } else if (argc > 2) {
        fprintf(stderr, "usage: %s [positive-element-count]\n", argv[0]);
        return 2;
    }

    int64_t *data = malloc(length * sizeof(*data));
    if (data == NULL) {
        fprintf(stderr, "allocation failed for %zu elements\n", length);
        return 1;
    }

    uint64_t state = UINT64_C(0x9e3779b97f4a7c15);
    for (size_t i = 0; i < length; ++i) {
        state ^= state >> 12;
        state ^= state << 25;
        state ^= state >> 27;
        state *= UINT64_C(0x2545f4914f6cdd1d);
        data[i] = (int64_t)(state % UINT64_C(1000000)) - 500000;
    }

    const clock_t start = clock();
    const int status = forge_i64_sort(data, length);
    const clock_t end = clock();

    if (status != 0) {
        free(data);
        fprintf(stderr, "sort failed\n");
        return 1;
    }

    printf("elements: %zu\n", length);
    printf("minimum: %" PRId64 "\n", data[0]);
    printf("maximum: %" PRId64 "\n", data[length - 1]);
    printf("cpu_time_seconds: %.6f\n", elapsed_seconds(start, end));

    free(data);
    return 0;
}
