#include "forge_expr.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double seconds(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

int main(int argc, char **argv) {
    size_t length = 5000000;
    if (argc > 1) length = (size_t)strtoull(argv[1], NULL, 10);
    if (length == 0) return 0;

    int64_t *a = malloc(length * sizeof(*a));
    int64_t *b = malloc(length * sizeof(*b));
    int64_t *out = malloc(length * sizeof(*out));
    if (!a || !b || !out) { free(a); free(b); free(out); return 1; }
    for (size_t i = 0; i < length; ++i) {
        a[i] = (int64_t)(i % 100000);
        b[i] = (int64_t)((i * 17U) % 1000U);
    }

    const double start = seconds();
    if (forge_i64_add(a, b, length, out) != 0) {
        free(a); free(b); free(out); return 1;
    }
    const double elapsed = seconds() - start;
    printf("add: %zu rows, %.6f s, checksum=%" PRId64 "\n",
           length, elapsed, out[length - 1]);

    free(a); free(b); free(out);
    return 0;
}
