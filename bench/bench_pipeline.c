#include "forge_pipeline.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec) +
           (double)(end->tv_nsec - start->tv_nsec) / 1000000000.0;
}

int main(int argc, char **argv) {
    size_t length = 5000000;
    if (argc > 1) {
        char *end = NULL;
        const unsigned long long parsed = strtoull(argv[1], &end, 10);
        if (end == argv[1] || *end != '\0' || parsed == 0 ||
            parsed > (unsigned long long)SIZE_MAX) {
            fprintf(stderr, "usage: %s [rows]\n", argv[0]);
            return 2;
        }
        length = (size_t)parsed;
    }

    forge_table table;
    forge_table_init(&table);
    if (forge_table_init_i64(&table, 2) != 0) {
        return 1;
    }
    for (size_t i = 0; i < length; ++i) {
        const int64_t key = (int64_t)(i % 1000);
        const int64_t value = (int64_t)(i % 100000);
        if (forge_table_append_i64(&table, 0, key) != 0 ||
            forge_table_append_i64(&table, 1, value) != 0) {
            forge_table_free(&table);
            return 1;
        }
    }

    forge_i64_pipeline pipeline;
    forge_i64_pipeline_init(&pipeline, &table);
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (forge_i64_pipeline_reset(&pipeline) != 0 ||
        forge_i64_pipeline_filter_ge(&pipeline, 0, 250) != 0 ||
        forge_i64_pipeline_filter_lt(&pipeline, 0, 750) != 0) {
        forge_i64_pipeline_free(&pipeline);
        forge_table_free(&table);
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("rows=%zu selected=%zu elapsed=%.6fs\n", length,
           pipeline.selection.length, elapsed_seconds(&start, &end));

    forge_i64_pipeline_free(&pipeline);
    forge_table_free(&table);
    return 0;
}
