#include "forge_csv.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_ingest_values(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    fputs("42\n-7\r\n9223372036854775807\n", stream);
    rewind(stream);

    forge_i64_column column;
    forge_i64_column_init(&column);

    assert(forge_csv_read_i64_column(stream, &column) == FORGE_CSV_OK);
    assert(column.length == 3);
    assert(column.data[0] == 42);
    assert(column.data[1] == -7);
    assert(column.data[2] == INT64_MAX);

    forge_i64_column_free(&column);
    fclose(stream);
}

static void test_reject_non_scalar_rows(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    fputs("10,20\n", stream);
    rewind(stream);

    forge_i64_column column;
    forge_i64_column_init(&column);

    assert(forge_csv_read_i64_column(stream, &column) == FORGE_CSV_INVALID_FIELD);
    assert(column.length == 0);

    forge_i64_column_free(&column);
    fclose(stream);
}

static void test_reject_invalid_integer(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    fputs("12x\n", stream);
    rewind(stream);

    forge_i64_column column;
    forge_i64_column_init(&column);

    assert(forge_csv_read_i64_column(stream, &column) == FORGE_CSV_INVALID_FIELD);
    assert(column.length == 0);

    forge_i64_column_free(&column);
    fclose(stream);
}

int main(void) {
    test_ingest_values();
    test_reject_non_scalar_rows();
    test_reject_invalid_integer();
    assert(forge_csv_read_i64_column(NULL, NULL) == FORGE_CSV_INVALID_ARGUMENT);
    return 0;
}
