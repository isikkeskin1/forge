#include "forge_scan.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

static int run_scan(size_t length, size_t rounds) {
    if (length > SIZE_MAX / sizeof(int64_t)) {
        return -1;
    }
    int64_t *data = malloc(length * sizeof(*data));
    if (data == NULL) {
        return -1;
    }
    for (size_t i = 0; i < length; ++i) {
        data[i] = (int64_t)(i % 1000);
    }

    forge_i64_scan_result result = {0, 0};
    const clock_t start = clock();
    for (size_t round = 0; round < rounds; ++round) {
        if (forge_i64_scan_ge(data, length, 500, &result) != 0) {
            free(data);
            return -1;
        }
    }
    const clock_t end = clock();
    const double seconds = elapsed_seconds(start, end);
    const double rows = (double)length * (double)rounds;

    printf("workload,rows,rounds,seconds,mrows_per_second,checksum\n");
    printf("scan_ge,%zu,%zu,%.6f,%.3f,%" PRId64 "\n",
           length, rounds, seconds,
           seconds > 0.0 ? rows / seconds / 1000000.0 : 0.0,
           result.sum + (int64_t)result.count);
    free(data);
    return 0;
}

int main(int argc, char **argv) {
    size_t length = 10000000;
    size_t rounds = 5;
    if (argc > 3) {
        fprintf(stderr, "usage: %s [rows] [rounds]\n", argv[0]);
        return 2;
    }
    if (argc >= 2) {
        char *end = NULL;
        const unsigned long long value = strtoull(argv[1], &end, 10);
        if (end == argv[1] || *end != '\0' || value == 0 || value > (unsigned long long)SIZE_MAX) {
            fprintf(stderr, "invalid row count\n");
            return 2;
        }
        length = (size_t)value;
    }
    if (argc == 3) {
        char *end = NULL;
        const unsigned long long value = strtoull(argv[2], &end, 10);
        if (end == argv[2] || *end != '\0' || value == 0 || value > (unsigned long long)SIZE_MAX) {
            fprintf(stderr, "invalid round count\n");
            return 2;
        }
        rounds = (size_t)value;
    }
    if (run_scan(length, rounds) != 0) {
        fprintf(stderr, "baseline workload failed\n");
        return 1;
    }
    return 0;
}
