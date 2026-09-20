#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double seconds_between(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

int main(int argc, char **argv) {
    size_t bytes = 64u * 1024u * 1024u;
    size_t rounds = 8;
    if (argc > 1) bytes = (size_t)strtoull(argv[1], NULL, 10);
    if (argc > 2) rounds = (size_t)strtoull(argv[2], NULL, 10);
    if (bytes < 4096 || rounds == 0) return 2;

    const size_t count = bytes / sizeof(uint64_t);
    uint64_t *data = malloc(count * sizeof(*data));
    if (data == NULL) return 1;
    for (size_t i = 0; i < count; ++i) data[i] = (uint64_t)i * UINT64_C(11400714819323198485);

    const size_t strides[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
    volatile uint64_t sink = 0;
    for (size_t s = 0; s < sizeof(strides) / sizeof(strides[0]); ++s) {
        const size_t stride = strides[s];
        struct timespec start, end;
        if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) { free(data); return 1; }
        uint64_t checksum = 0;
        size_t accesses = 0;
        for (size_t round = 0; round < rounds; ++round) {
            for (size_t i = 0; i < count; i += stride) {
                checksum += data[i];
                ++accesses;
            }
        }
        if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) { free(data); return 1; }
        sink ^= checksum;
        const double elapsed = seconds_between(start, end);
        const double ns_per_access = elapsed * 1e9 / (double)accesses;
        printf("stride elements=%zu bytes=%zu accesses=%zu ns/access=%.3f checksum=%" PRIu64 "\n",
               stride, stride * sizeof(uint64_t), accesses, ns_per_access, checksum);
    }

    if (sink == UINT64_MAX) fprintf(stderr, "sink=%" PRIu64 "\n", sink);
    free(data);
    return 0;
}
