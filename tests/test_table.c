#include "forge_table.h"

#include <assert.h>
#include <stdint.h>

static void test_append_rows(void) {
    forge_table table;
    forge_table_init(&table);

    assert(forge_table_init_i64(&table, 3) == 0);
    assert(forge_table_append_i64(&table, 0, 10) == 0);
    assert(forge_table_append_i64(&table, 1, 100) == 0);
    assert(forge_table_append_i64(&table, 2, 1000) == 0);
    assert(table.row_count == 1);

    assert(forge_table_append_i64(&table, 0, 20) == 0);
    assert(forge_table_append_i64(&table, 1, 200) == 0);
    assert(forge_table_append_i64(&table, 2, 2000) == 0);
    assert(table.row_count == 2);
    assert(forge_table_validate(&table) == 0);

    assert(table.columns[0].data[0] == 10);
    assert(table.columns[1].data[1] == 200);
    assert(table.columns[2].data[0] == 1000);

    forge_table_free(&table);
}

static void test_partial_row_is_rejected(void) {
    forge_table table;
    forge_table_init(&table);
    assert(forge_table_init_i64(&table, 2) == 0);

    assert(forge_table_append_i64(&table, 0, 7) == 0);
    assert(forge_table_validate(&table) != 0);
    assert(forge_table_append_i64(&table, 0, 8) != 0);
    assert(forge_table_append_i64(&table, 1, 70) == 0);
    assert(forge_table_validate(&table) == 0);
    assert(table.row_count == 1);

    forge_table_free(&table);
}

static void test_invalid_inputs(void) {
    forge_table table;
    forge_table_init(&table);

    assert(forge_table_init_i64(NULL, 2) != 0);
    assert(forge_table_init_i64(&table, 0) != 0);
    assert(forge_table_append_i64(NULL, 0, 1) != 0);
    assert(forge_table_validate(NULL) != 0);
    assert(forge_table_validate(&table) == 0);

    forge_table_free(&table);
}

int main(void) {
    test_append_rows();
    test_partial_row_is_rejected();
    test_invalid_inputs();
    return 0;
}
