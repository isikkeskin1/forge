#include "forge_buffer.h"

#include <stdlib.h>
#include <string.h>

static int forge_size_add_overflow(size_t a, size_t b) {
    return a > SIZE_MAX - b;
}

void forge_buffer_init(forge_buffer *buffer) {
    if (buffer == NULL) {
        return;
    }
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
}

void forge_buffer_free(forge_buffer *buffer) {
    if (buffer == NULL) {
        return;
    }
    free(buffer->data);
    forge_buffer_init(buffer);
}

int forge_buffer_reserve(forge_buffer *buffer, size_t capacity) {
    if (buffer == NULL || capacity <= buffer->capacity) {
        return buffer != NULL ? 0 : -1;
    }

    uint8_t *next = realloc(buffer->data, capacity);
    if (next == NULL) {
        return -1;
    }

    buffer->data = next;
    buffer->capacity = capacity;
    return 0;
}

int forge_buffer_append(forge_buffer *buffer, const void *data, size_t size) {
    if (buffer == NULL || (data == NULL && size != 0)) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    if (forge_size_add_overflow(buffer->size, size)) {
        return -1;
    }

    const size_t required = buffer->size + size;
    if (required > buffer->capacity) {
        size_t next_capacity = buffer->capacity == 0 ? 256 : buffer->capacity;
        while (next_capacity < required) {
            if (next_capacity > SIZE_MAX / 2) {
                next_capacity = required;
                break;
            }
            next_capacity *= 2;
        }
        if (forge_buffer_reserve(buffer, next_capacity) != 0) {
            return -1;
        }
    }

    memcpy(buffer->data + buffer->size, data, size);
    buffer->size = required;
    return 0;
}
