#ifndef FORGE_BUFFER_H
#define FORGE_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t size;
    size_t capacity;
} forge_buffer;

void forge_buffer_init(forge_buffer *buffer);
void forge_buffer_free(forge_buffer *buffer);
int forge_buffer_reserve(forge_buffer *buffer, size_t capacity);
int forge_buffer_append(forge_buffer *buffer, const void *data, size_t size);

#endif
