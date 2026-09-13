#ifndef FORGE_EXPR_H
#define FORGE_EXPR_H

#include <stddef.h>
#include <stdint.h>

#include "forge_selection.h"

/* Evaluate a binary integer expression into a caller-owned output column. */
int forge_i64_add(const int64_t *left, const int64_t *right,
                  size_t length, int64_t *output);
int forge_i64_sub(const int64_t *left, const int64_t *right,
                  size_t length, int64_t *output);
int forge_i64_mul(const int64_t *left, const int64_t *right,
                  size_t length, int64_t *output);

/* Evaluate a binary expression only for rows in a selection. */
int forge_i64_add_selected(const int64_t *left, const int64_t *right,
                           const forge_selection *selection,
                           size_t length, int64_t *output);

#endif
