#ifndef FORGE_PIPELINE_H
#define FORGE_PIPELINE_H

#include <stddef.h>
#include <stdint.h>

#include "forge_selection.h"
#include "forge_table.h"

typedef struct {
    const forge_table *table;
    forge_selection selection;
} forge_i64_pipeline;

void forge_i64_pipeline_init(forge_i64_pipeline *pipeline,
                             const forge_table *table);
void forge_i64_pipeline_free(forge_i64_pipeline *pipeline);

/* Start with every row selected. */
int forge_i64_pipeline_reset(forge_i64_pipeline *pipeline);

/* Restrict the current selection with a column predicate. */
int forge_i64_pipeline_filter_ge(forge_i64_pipeline *pipeline,
                                 size_t column, int64_t threshold);
int forge_i64_pipeline_filter_lt(forge_i64_pipeline *pipeline,
                                 size_t column, int64_t threshold);
int forge_i64_pipeline_filter_eq(forge_i64_pipeline *pipeline,
                                 size_t column, int64_t value);

/* Materialize one selected column in selection order. */
size_t forge_i64_pipeline_project(const forge_i64_pipeline *pipeline,
                                  size_t column, int64_t *output);

#endif
