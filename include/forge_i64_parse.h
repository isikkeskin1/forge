#ifndef FORGE_I64_PARSE_H
#define FORGE_I64_PARSE_H

#include <stdint.h>

/* Parse a complete base-10 signed integer without modifying the input. */
int forge_i64_parse(const char *text, int64_t *value);

#endif
