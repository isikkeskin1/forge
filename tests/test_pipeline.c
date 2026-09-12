#include "forge_pipeline.h"

#include <assert.h>
#include <stdint.h>

static void append_row(forge_table *table, int64_t a, int64_t b) {
    assert(forge_table_append_i64(table, 0, a) == 0);
    assert(forge_table_append_i64(table, 1, b) == 0);
}

static void test_composed_filters(void) {
    forge_table table;
    forge_table_init(&table);
    assert(forge_table_init_i64(&table, 2) == 0);
    append_row(&table, 1, 100);
    append_row(&table, 5, 10);
    append_row(&table, 8, 80);
    append_row(&table, 12, 120);
    append_row(&table, 20, 50);
    assert(forge_table_validate(&table) == 0);

    forge_i64_pipeline pipeline;
    forge_i64_pipeline_init(&pipeline, &table);
    assert(forge_i64_pipeline_reset(&pipeline) == 0);
    assert(forge_i64_pipeline_filter_ge(&pipeline, 0, 8) == 0);
    assert(pipeline.selection.length == 3);
    assert(forge_i64_pipeline_filter_lt(&pipeline, 1, 100) == 0);
    assert(pipeline.selection.length == 1);
    assert(pipeline.selection.indices[0] == 2);

    int64_t output[3] = {0, 0, 0};
    assert(forge_i64_pipeline_project(&pipeline, 0, output) == 1);
    assert(output[0] == 8);

    forge_i64_pipeline_free(&pipeline);
    forge_table_free(&table);
}

static void test_empty_and_invalid(void) {
    forge_table table;
    forge_table_init(&table);
    assert(forge_table_init_i64(&table, 1) == 0);

    forge_i64_pipeline pipeline;
    forge_i64_pipeline_init(&pipeline, &table);
    assert(forge_i64_pipeline_reset(&pipeline) == 0);
    assert(pipeline.selection.length == 0);
    assert(forge_i64_pipeline_filter_eq(&pipeline, 0, 7) == 0);
    assert(pipeline.selection.length == 0);
    assert(forge_i64_pipeline_filter_ge(&pipeline, 1, 0) != 0);
    assert(forge_i64_pipeline_project(&pipeline, 1, NULL) == 0);

    forge_i64_pipeline_free(&pipeline);
    forge_table_free(&table);
}

int main(void) {
    test_composed_filters();
    test_empty_and_invalid();
    return 0;
}
