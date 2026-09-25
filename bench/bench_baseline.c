#include "forge_scan.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

static int run_scan(size_t length, size_t rounds, int64_t threshold) {
    if (length > SIZE_MAX / sizeof(int64_t)) {
        return -1;
    }
    int64_t *data = malloc(length * sizeof(*data));
    if (data == NULL && length != 0) {
        return -1;
    }
    for (size_t i = 0; i < length; ++i) {
        data[i] = (int64_t)((i * 17) % 1009) - 504;
    }

    forge_i64_scan_result expected = {0, 0};
    double best_seconds = 0.0;
    for (size_t round = 0; round < rounds; ++round) {
        forge_i64_scan_result result = {0, 0};
        const clock_t start = clock();
        if (forge_i64_scan_ge(data, length, threshold, &result) != 0) {
            free(data);
            return -1;
        }
        const double seconds = elapsed_seconds(start, clock());
        if (round == 0) {
            expected = result;
            best_seconds = seconds;
        } else if (result.count != expected.count || result.sum != expected.sum) {
            free(data);
            return -1;
        } else if (seconds < best_seconds) {
            best_seconds = seconds;
        }
    }

    printf("engine,rows,rounds,threshold,seconds,rows_per_second,count,sum\n");
    printf("forge,%zu,%zu,%" PRId64 ",%.9f,%.3f,%zu,%" PRId64 "\n",
           length, rounds, threshold, best_seconds,
           best_seconds > 0.0 ? (double)length / best_seconds : 0.0,
           expected.count, expected.sum);
    free(data);
    return 0;
}

int main(int argc, char **argv) {
    size_t length = 10000000;
    size_t rounds = 5;
    int64_t threshold = 0;
    if (argc > 4) {
        fprintf(stderr, "usage: %s [rows] [rounds] [threshold]\n", argv[0]);
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
    if (argc >= 3) {
        char *end = NULL;
        const unsigned long long value = strtoull(argv[2], &end, 10);
        if (end == argv[2] || *end != '\0' || value == 0 || value > (unsigned long long)SIZE_MAX) {
            fprintf(stderr, "invalid round count\n");
            return 2;
        }
        rounds = (size_t)value;
    }
    if (argc == 4) {
        char *end = NULL;
        const long long value = strtoll(argv[3], &end, 10);
        if (end == argv[3] || *end != '\0') {
            fprintf(stderr, "invalid threshold\n");
            return 2;
        }
        threshold = (int64_t)value;
    }
    if (run_scan(length, rounds, threshold) != 0) {
        fprintf(stderr, "baseline workload failed\n");
        return 1;
    }
    return 0;
}
