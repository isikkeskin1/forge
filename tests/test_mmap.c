#define _POSIX_C_SOURCE 200809L

#include "forge_mmap.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
#include <fcntl.h>
#include <unistd.h>

static void write_all(int fd, const char *data, size_t length) {
    size_t offset = 0;
    while (offset < length) {
        const ssize_t written = write(fd, data + offset, length - offset);
        assert(written > 0);
        offset += (size_t)written;
    }
}

static void test_map_contents(void) {
    char path[] = "/tmp/forge-mmap-XXXXXX";
    const char payload[] = "17,23,42\n";
    int fd = mkstemp(path);
    assert(fd >= 0);
    write_all(fd, payload, sizeof(payload) - 1);
    assert(close(fd) == 0);

    forge_mapped_file mapped;
    forge_mapped_file_init(&mapped);
    assert(forge_mapped_file_open(path, &mapped) == FORGE_MMAP_OK);
    assert(mapped.length == sizeof(payload) - 1);
    assert(mapped.data != NULL);
    assert(memcmp(mapped.data, payload, mapped.length) == 0);

    forge_mapped_file_close(&mapped);
    assert(mapped.data == NULL);
    assert(mapped.length == 0);
    assert(mapped.fd == -1);
    forge_mapped_file_close(&mapped);

    assert(unlink(path) == 0);
}

static void test_empty_file(void) {
    char path[] = "/tmp/forge-mmap-empty-XXXXXX";
    int fd = mkstemp(path);
    assert(fd >= 0);
    assert(close(fd) == 0);

    forge_mapped_file mapped;
    forge_mapped_file_init(&mapped);
    assert(forge_mapped_file_open(path, &mapped) == FORGE_MMAP_OK);
    assert(mapped.length == 0);
    assert(mapped.data == NULL);
    assert(mapped.fd >= 0);

    forge_mapped_file_close(&mapped);
    assert(unlink(path) == 0);
}

static void test_replace_mapping(void) {
    char first[] = "/tmp/forge-mmap-first-XXXXXX";
    char second[] = "/tmp/forge-mmap-second-XXXXXX";
    int first_fd = mkstemp(first);
    int second_fd = mkstemp(second);
    assert(first_fd >= 0 && second_fd >= 0);
    write_all(first_fd, "abc", 3);
    write_all(second_fd, "xyz123", 6);
    assert(close(first_fd) == 0);
    assert(close(second_fd) == 0);

    forge_mapped_file mapped;
    forge_mapped_file_init(&mapped);
    assert(forge_mapped_file_open(first, &mapped) == FORGE_MMAP_OK);
    assert(mapped.length == 3);
    assert(forge_mapped_file_open(second, &mapped) == FORGE_MMAP_OK);
    assert(mapped.length == 6);
    assert(memcmp(mapped.data, "xyz123", 6) == 0);

    forge_mapped_file_close(&mapped);
    assert(unlink(first) == 0);
    assert(unlink(second) == 0);
}

static void test_errors(void) {
    forge_mapped_file mapped;
    forge_mapped_file_init(&mapped);

    assert(forge_mapped_file_open(NULL, &mapped) == FORGE_MMAP_INVALID_ARGUMENT);
    assert(forge_mapped_file_open("", &mapped) == FORGE_MMAP_INVALID_ARGUMENT);
    assert(forge_mapped_file_open("/tmp/forge-this-file-does-not-exist", &mapped) == FORGE_MMAP_OPEN_ERROR);
    assert(forge_mapped_file_open("anything", NULL) == FORGE_MMAP_INVALID_ARGUMENT);

    forge_mapped_file_close(&mapped);
    forge_mapped_file_close(NULL);
}

int main(void) {
    test_map_contents();
    test_empty_file();
    test_replace_mapping();
    test_errors();
    return 0;
}

#else

int main(void) {
    forge_mapped_file mapped;
    forge_mapped_file_init(&mapped);
    assert(forge_mapped_file_open("anything", &mapped) == FORGE_MMAP_UNSUPPORTED);
    forge_mapped_file_close(&mapped);
    return 0;
}

#endif
