#ifndef FORGE_GROUP_H
#define FORGE_GROUP_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int64_t key;
    int64_t sum;
    size_t count;
    uint8_t occupied;
} forge_group_entry;

typedef struct {
    forge_group_entry *entries;
    size_t capacity;
    size_t size;
} forge_i64_group;

void forge_i64_group_init(forge_i64_group *group);
void forge_i64_group_free(forge_i64_group *group);
int forge_i64_group_reserve(forge_i64_group *group, size_t capacity);
int forge_i64_group_add(forge_i64_group *group, int64_t key, int64_t value);
int forge_i64_group_get(const forge_i64_group *group, int64_t key,
                        size_t *count, int64_t *sum);

/* Group one integer column and aggregate a second integer column. */
int forge_i64_group_scan(const int64_t *keys, const int64_t *values,
                         size_t length, forge_i64_group *group);

#endif
