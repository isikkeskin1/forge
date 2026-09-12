#ifndef FORGE_CSV_H
#define FORGE_CSV_H

#include <stddef.h>
#include <stdio.h>

#include "forge_column.h"

typedef enum {
    FORGE_CSV_OK = 0,
    FORGE_CSV_INVALID_ARGUMENT = 1,
    FORGE_CSV_IO_ERROR = 2,
    FORGE_CSV_UNTERMINATED_QUOTE = 3,
    FORGE_CSV_INVALID_FIELD = 4
} forge_csv_status;

typedef struct {
    size_t fields;
    size_t records;
} forge_csv_stats;

/* Scan a CSV stream without retaining the parsed records. */
forge_csv_status forge_csv_scan(FILE *stream, forge_csv_stats *stats);

/* Read a single-column CSV stream into a signed 64-bit column. */
forge_csv_status forge_csv_read_i64_column(FILE *stream, forge_i64_column *column);

#endif
