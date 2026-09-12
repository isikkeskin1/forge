#ifndef FORGE_SCAN_H
#define FORGE_SCAN_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t count;
    int64_t sum;
} forge_i64_scan_result;

/* Execute count and sum for one predicate in a single column pass. */
int forge_i64_scan_ge(const int64_t *data, size_t length, int64_t threshold,
                      forge_i64_scan_result *result);

#endif
