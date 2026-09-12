#include "forge_pipeline.h"

#include <stdlib.h>

static int forge_pipeline_valid_column(const forge_i64_pipeline *pipeline,
                                       size_t column) {
    return pipeline != NULL && pipeline->table != NULL &&
           pipeline->table->columns != NULL &&
           column < pipeline->table->column_count;
}

void forge_i64_pipeline_init(forge_i64_pipeline *pipeline,
                             const forge_table *table) {
    if (pipeline == NULL) {
        return;
    }
    pipeline->table = table;
    forge_selection_init(&pipeline->selection);
}

void forge_i64_pipeline_free(forge_i64_pipeline *pipeline) {
    if (pipeline == NULL) {
        return;
    }
    forge_selection_free(&pipeline->selection);
    pipeline->table = NULL;
}

int forge_i64_pipeline_reset(forge_i64_pipeline *pipeline) {
    if (pipeline == NULL || pipeline->table == NULL ||
        (pipeline->table->columns == NULL && pipeline->table->row_count != 0)) {
        return -1;
    }

    forge_selection_free(&pipeline->selection);
    forge_selection_init(&pipeline->selection);
    if (pipeline->table->row_count == 0) {
        return 0;
    }
    if (forge_selection_reserve(&pipeline->selection,
                                pipeline->table->row_count) != 0) {
        return -1;
    }
    for (size_t i = 0; i < pipeline->table->row_count; ++i) {
        pipeline->selection.indices[i] = i;
    }
    pipeline->selection.length = pipeline->table->row_count;
    return 0;
}

static int forge_pipeline_filter(forge_i64_pipeline *pipeline, size_t column,
                                 int64_t value, int predicate) {
    if (!forge_pipeline_valid_column(pipeline, column) ||
        forge_table_validate(pipeline->table) != 0) {
        return -1;
    }

    forge_selection matches;
    forge_selection_init(&matches);
    const int64_t *data = pipeline->table->columns[column].data;
    int rc;
    if (predicate == 0) {
        rc = forge_i64_select_ge(data, pipeline->table->row_count, value, &matches);
    } else if (predicate == 1) {
        rc = forge_i64_select_lt(data, pipeline->table->row_count, value, &matches);
    } else {
        rc = forge_i64_select_eq(data, pipeline->table->row_count, value, &matches);
    }
    if (rc != 0) {
        forge_selection_free(&matches);
        return -1;
    }

    forge_selection filtered;
    forge_selection_init(&filtered);
    rc = forge_selection_intersect(&pipeline->selection, &matches, &filtered);
    forge_selection_free(&matches);
    if (rc != 0) {
        forge_selection_free(&filtered);
        return -1;
    }

    forge_selection_free(&pipeline->selection);
    pipeline->selection = filtered;
    return 0;
}

int forge_i64_pipeline_filter_ge(forge_i64_pipeline *pipeline,
                                 size_t column, int64_t threshold) {
    return forge_pipeline_filter(pipeline, column, threshold, 0);
}

int forge_i64_pipeline_filter_lt(forge_i64_pipeline *pipeline,
                                 size_t column, int64_t threshold) {
    return forge_pipeline_filter(pipeline, column, threshold, 1);
}

int forge_i64_pipeline_filter_eq(forge_i64_pipeline *pipeline,
                                 size_t column, int64_t value) {
    return forge_pipeline_filter(pipeline, column, value, 2);
}

size_t forge_i64_pipeline_project(const forge_i64_pipeline *pipeline,
                                  size_t column, int64_t *output) {
    if (!forge_pipeline_valid_column(pipeline, column) ||
        (output == NULL && pipeline->selection.length != 0)) {
        return 0;
    }
    const forge_i64_column *source = &pipeline->table->columns[column];
    return forge_i64_gather(source->data, source->length,
                            &pipeline->selection, output);
}
