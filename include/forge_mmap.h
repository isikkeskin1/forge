#ifndef FORGE_MMAP_H
#define FORGE_MMAP_H

#include <stddef.h>

typedef enum {
    FORGE_MMAP_OK = 0,
    FORGE_MMAP_INVALID_ARGUMENT = 1,
    FORGE_MMAP_OPEN_ERROR = 2,
    FORGE_MMAP_STAT_ERROR = 3,
    FORGE_MMAP_MAP_ERROR = 4,
    FORGE_MMAP_UNSUPPORTED = 5
} forge_mmap_status;

typedef struct {
    const unsigned char *data;
    size_t length;
    int fd;
} forge_mapped_file;

void forge_mapped_file_init(forge_mapped_file *mapped);

/* Map an existing file read-only. Empty files are valid and yield data == NULL. */
forge_mmap_status forge_mapped_file_open(const char *path, forge_mapped_file *mapped);

/* Release the mapping and backing descriptor. Safe to call on an initialized object. */
void forge_mapped_file_close(forge_mapped_file *mapped);

#endif
