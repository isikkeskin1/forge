#include "forge_mmap.h"

#include <limits.h>

#if defined(_WIN32)

void forge_mapped_file_init(forge_mapped_file *mapped) {
    if (mapped == NULL) {
        return;
    }
    mapped->data = NULL;
    mapped->length = 0;
    mapped->fd = -1;
}

forge_mmap_status forge_mapped_file_open(const char *path, forge_mapped_file *mapped) {
    if (path == NULL || mapped == NULL) {
        return FORGE_MMAP_INVALID_ARGUMENT;
    }
    forge_mapped_file_init(mapped);
    return FORGE_MMAP_UNSUPPORTED;
}

void forge_mapped_file_close(forge_mapped_file *mapped) {
    forge_mapped_file_init(mapped);
}

#else

#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void forge_mapped_file_init(forge_mapped_file *mapped) {
    if (mapped == NULL) {
        return;
    }
    mapped->data = NULL;
    mapped->length = 0;
    mapped->fd = -1;
}

void forge_mapped_file_close(forge_mapped_file *mapped) {
    if (mapped == NULL) {
        return;
    }
    if (mapped->data != NULL && mapped->length != 0) {
        (void)munmap((void *)mapped->data, mapped->length);
    }
    if (mapped->fd >= 0) {
        (void)close(mapped->fd);
    }
    forge_mapped_file_init(mapped);
}

forge_mmap_status forge_mapped_file_open(const char *path, forge_mapped_file *mapped) {
    if (path == NULL || mapped == NULL || path[0] == '\0') {
        return FORGE_MMAP_INVALID_ARGUMENT;
    }

    /* Opening replaces any existing mapping owned by this object. */
    forge_mapped_file_close(mapped);

    const int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return FORGE_MMAP_OPEN_ERROR;
    }

    struct stat info;
    if (fstat(fd, &info) != 0) {
        (void)close(fd);
        return FORGE_MMAP_STAT_ERROR;
    }
    if (info.st_size < 0 || (uintmax_t)info.st_size > (uintmax_t)SIZE_MAX) {
        (void)close(fd);
        return FORGE_MMAP_STAT_ERROR;
    }

    mapped->fd = fd;
    mapped->length = (size_t)info.st_size;
    if (mapped->length == 0) {
        mapped->data = NULL;
        return FORGE_MMAP_OK;
    }

    void *address = mmap(NULL, mapped->length, PROT_READ, MAP_PRIVATE, fd, 0);
    if (address == MAP_FAILED) {
        (void)close(fd);
        forge_mapped_file_init(mapped);
        return FORGE_MMAP_MAP_ERROR;
    }

    mapped->data = (const unsigned char *)address;
    return FORGE_MMAP_OK;
}

#endif
