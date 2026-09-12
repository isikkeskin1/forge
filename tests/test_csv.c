#include "forge_csv.h"

#include <assert.h>
#include <stdio.h>

static void test_basic_records(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    fputs("name,score\nalice,42\nbob,37,\n", stream);
    rewind(stream);

    forge_csv_stats stats;
    assert(forge_csv_scan(stream, &stats) == 0);
    assert(stats.records == 3);
    assert(stats.fields == 7);

    fclose(stream);
}

static void test_quoted_fields(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    fputs("name,note\n\"Ada, Lovelace\",\"line one\nline two\"\n", stream);
    rewind(stream);

    forge_csv_stats stats;
    assert(forge_csv_scan(stream, &stats) == 0);
    assert(stats.records == 2);
    assert(stats.fields == 4);

    fclose(stream);
}

static void test_empty_input(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    forge_csv_stats stats;
    assert(forge_csv_scan(stream, &stats) == 0);
    assert(stats.records == 0);
    assert(stats.fields == 0);

    fclose(stream);
}

int main(void) {
    test_basic_records();
    test_quoted_fields();
    test_empty_input();
    assert(forge_csv_scan(NULL, NULL) != 0);
    return 0;
}
