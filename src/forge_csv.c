#include "forge_csv.h"

#include "forge_i64_parse.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

forge_csv_status forge_csv_scan(FILE *stream, forge_csv_stats *stats) {
    if (stream == NULL || stats == NULL) {
        return FORGE_CSV_INVALID_ARGUMENT;
    }

    stats->fields = 0;
    stats->records = 0;

    int quoted = 0;
    int pending_quote = 0;
    int record_started = 0;
    int field_has_data = 0;

    for (;;) {
        const int ch = fgetc(stream);
        if (ch == EOF) {
            if (ferror(stream)) {
                return FORGE_CSV_IO_ERROR;
            }
            if (quoted) {
                return FORGE_CSV_UNTERMINATED_QUOTE;
            }
            if (record_started) {
                ++stats->fields;
                ++stats->records;
            }
            return FORGE_CSV_OK;
        }

        record_started = 1;

        if (quoted) {
            if (pending_quote) {
                if (ch == '"') {
                    pending_quote = 0;
                    field_has_data = 1;
                    continue;
                }
                quoted = 0;
                pending_quote = 0;
            } else if (ch == '"') {
                pending_quote = 1;
                continue;
            } else {
                field_has_data = 1;
                continue;
            }
        }

        if (ch == '"' && !field_has_data) {
            quoted = 1;
            field_has_data = 1;
        } else if (ch == ',') {
            ++stats->fields;
            field_has_data = 0;
        } else if (ch == '\n') {
            ++stats->fields;
            ++stats->records;
            record_started = 0;
            field_has_data = 0;
        } else if (ch != '\r') {
            field_has_data = 1;
        }
    }
}

forge_csv_status forge_csv_read_i64_column(FILE *stream, forge_i64_column *column) {
    if (stream == NULL || column == NULL) {
        return FORGE_CSV_INVALID_ARGUMENT;
    }

    char *line = NULL;
    size_t length = 0;
    size_t capacity = 0;

    for (;;) {
        const int ch = fgetc(stream);
        if (ch == EOF) {
            if (ferror(stream)) {
                free(line);
                return FORGE_CSV_IO_ERROR;
            }
            if (length == 0) {
                free(line);
                return FORGE_CSV_OK;
            }
        } else if (ch == '\r') {
            continue;
        } else if (ch != '\n') {
            if (length == SIZE_MAX) {
                free(line);
                return FORGE_CSV_INVALID_FIELD;
            }
            if (length + 2 > capacity) {
                size_t next = capacity == 0 ? 64 : capacity;
                while (next < length + 2) {
                    if (next > SIZE_MAX / 2) {
                        next = length + 2;
                        break;
                    }
                    next *= 2;
                }
                char *resized = realloc(line, next);
                if (resized == NULL) {
                    free(line);
                    return FORGE_CSV_IO_ERROR;
                }
                line = resized;
                capacity = next;
            }
            line[length++] = (char)ch;
            continue;
        }

        if (length == 0 || line[0] == '"') {
            free(line);
            return FORGE_CSV_INVALID_FIELD;
        }
        for (size_t i = 0; i < length; ++i) {
            if (line[i] == ',') {
                free(line);
                return FORGE_CSV_INVALID_FIELD;
            }
        }

        line[length] = '\0';
        int64_t value = 0;
        if (forge_i64_parse(line, &value) != 0 || forge_i64_column_append(column, value) != 0) {
            free(line);
            return FORGE_CSV_INVALID_FIELD;
        }
        length = 0;

        if (ch == EOF) {
            break;
        }
    }

    free(line);
    return FORGE_CSV_OK;
}
