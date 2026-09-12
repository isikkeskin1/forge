#ifndef FORGE_CSV_H
#define FORGE_CSV_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    size_t fields;
    size_t records;
} forge_csv_stats;

/* Scan a CSV stream without retaining the parsed records. */
int forge_csv_scan(FILE *stream, forge_csv_stats *stats);

#endif
