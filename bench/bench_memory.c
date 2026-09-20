#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double seconds_between(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

static uint64_t next_random(uint64_t *state) {
    uint64_t x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *state = x;
    return x;
}

int main(int argc, char **argv) {
    size_t bytes = 256u * 1024u * 1024u;
    size_t passes = 5;
    if (argc > 1) {
        bytes = (size_t)strtoull(argv[1], NULL, 10);
    }
    if (argc > 2) {
        passes = (size_t)strtoull(argv[2], NULL, 10);
    }
    if (bytes < sizeof(uint64_t) || passes == 0) {
        fprintf(stderr, "usage: %s [bytes>=8] [passes>=1]\n", argv[0]);
        return 2;
    }

    const size_t count = bytes / sizeof(uint64_t);
    uint64_t *data = malloc(count * sizeof(*data));
    if (data == NULL) {
        fprintf(stderr, "allocation failed for %zu bytes\n", bytes);
        return 1;
    }

    uint64_t state = UINT64_C(0x9e3779b97f4a7c15);
    for (size_t i = 0; i < count; ++i) {
        data[i] = next_random(&state);
    }

    volatile uint64_t checksum = 0;
    struct timespec start;
    struct timespec end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        free(data);
        return 1;
    }
    for (size_t pass = 0; pass < passes; ++pass) {
        uint64_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += data[i];
        }
        checksum ^= sum;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        free(data);
        return 1;
    }

    const double elapsed = seconds_between(start, end);
    const double gib = (double)(count * sizeof(*data)) * (double)passes /
                       (1024.0 * 1024.0 * 1024.0);
    printf("memory_sweep bytes=%zu passes=%zu seconds=%.6f GiB/s=%.3f checksum=%" PRIu64 "\n",
           count * sizeof(*data), passes, elapsed, gib / elapsed, checksum);
    free(data);
    return 0;
}
