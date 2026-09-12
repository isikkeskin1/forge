#include "forge_group.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t forge_mix_u64(uint64_t value) {
    value ^= value >> 30;
    value *= UINT64_C(0xbf58476d1ce4e5b9);
    value ^= value >> 27;
    value *= UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

static size_t forge_next_power_of_two(size_t value) {
    if (value <= 1) {
        return 1;
    }
    --value;
    for (size_t shift = 1; shift < sizeof(value) * 8; shift <<= 1) {
        value |= value >> shift;
    }
    return value + 1;
}

void forge_i64_group_init(forge_i64_group *group) {
    if (group == NULL) {
        return;
    }
    group->entries = NULL;
    group->capacity = 0;
    group->size = 0;
}

void forge_i64_group_free(forge_i64_group *group) {
    if (group == NULL) {
        return;
    }
    free(group->entries);
    forge_i64_group_init(group);
}

static int forge_i64_group_insert_existing(forge_group_entry *entries,
                                           size_t capacity,
                                           forge_group_entry entry) {
    const size_t mask = capacity - 1;
    size_t slot = (size_t)(forge_mix_u64((uint64_t)entry.key) & (uint64_t)mask);
    while (entries[slot].occupied != 0) {
        slot = (slot + 1) & mask;
    }
    entries[slot] = entry;
    return 0;
}

int forge_i64_group_reserve(forge_i64_group *group, size_t requested) {
    if (group == NULL) {
        return -1;
    }
    if (requested <= group->capacity) {
        return 0;
    }
    if (requested > SIZE_MAX / sizeof(*group->entries)) {
        return -1;
    }

    const size_t capacity = forge_next_power_of_two(requested);
    if (capacity < requested || capacity < 8 ||
        capacity > SIZE_MAX / sizeof(*group->entries)) {
        return -1;
    }

    forge_group_entry *entries = calloc(capacity, sizeof(*entries));
    if (entries == NULL) {
        return -1;
    }

    if (group->entries != NULL) {
        for (size_t i = 0; i < group->capacity; ++i) {
            if (group->entries[i].occupied != 0) {
                forge_i64_group_insert_existing(entries, capacity, group->entries[i]);
            }
        }
    }

    free(group->entries);
    group->entries = entries;
    group->capacity = capacity;
    return 0;
}

static int forge_i64_group_rehash_if_needed(forge_i64_group *group) {
    if (group->capacity == 0) {
        return forge_i64_group_reserve(group, 8);
    }
    /* Keep load below 70% so linear probing stays bounded. */
    if (group->size + 1 <= (group->capacity * 7) / 10) {
        return 0;
    }
    if (group->capacity > SIZE_MAX / 2) {
        return -1;
    }
    return forge_i64_group_reserve(group, group->capacity * 2);
}

int forge_i64_group_add(forge_i64_group *group, int64_t key, int64_t value) {
    if (group == NULL) {
        return -1;
    }
    if (forge_i64_group_rehash_if_needed(group) != 0) {
        return -1;
    }

    const size_t mask = group->capacity - 1;
    size_t slot = (size_t)(forge_mix_u64((uint64_t)key) & (uint64_t)mask);
    for (;;) {
        forge_group_entry *entry = &group->entries[slot];
        if (entry->occupied == 0) {
            entry->key = key;
            entry->sum = value;
            entry->count = 1;
            entry->occupied = 1;
            ++group->size;
            return 0;
        }
        if (entry->key == key) {
            if ((value > 0 && entry->sum > INT64_MAX - value) ||
                (value < 0 && entry->sum < INT64_MIN - value) ||
                entry->count == SIZE_MAX) {
                return -1;
            }
            entry->sum += value;
            ++entry->count;
            return 0;
        }
        slot = (slot + 1) & mask;
    }
}

int forge_i64_group_get(const forge_i64_group *group, int64_t key,
                        size_t *count, int64_t *sum) {
    if (group == NULL || group->capacity == 0 ||
        (count == NULL && sum == NULL)) {
        return -1;
    }

    const size_t mask = group->capacity - 1;
    size_t slot = (size_t)(forge_mix_u64((uint64_t)key) & (uint64_t)mask);
    for (;;) {
        const forge_group_entry *entry = &group->entries[slot];
        if (entry->occupied == 0) {
            return 1;
        }
        if (entry->key == key) {
            if (count != NULL) {
                *count = entry->count;
            }
            if (sum != NULL) {
                *sum = entry->sum;
            }
            return 0;
        }
        slot = (slot + 1) & mask;
    }
}

int forge_i64_group_scan(const int64_t *keys, const int64_t *values,
                         size_t length, forge_i64_group *group) {
    if ((keys == NULL || values == NULL) && length != 0) {
        return -1;
    }
    if (group == NULL) {
        return -1;
    }
    for (size_t i = 0; i < length; ++i) {
        if (forge_i64_group_add(group, keys[i], values[i]) != 0) {
            return -1;
        }
    }
    return 0;
}
