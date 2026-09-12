#include "forge_csv.h"

#include <stddef.h>

int forge_csv_scan(FILE *stream, forge_csv_stats *stats) {
    if (stream == NULL || stats == NULL) {
        return -1;
    }

    stats->fields = 0;
    stats->records = 0;

    int quoted = 0;
    int saw_data = 0;
    int field_has_data = 0;
    int pending_quote = 0;

    for (;;) {
        const int ch = fgetc(stream);
        if (ch == EOF) {
            if (ferror(stream)) {
                return -1;
            }
            if (saw_data || field_has_data) {
                ++stats->fields;
                ++stats->records;
            }
            return 0;
        }

        saw_data = 1;

        if (quoted) {
            if (pending_quote) {
                if (ch == '"') {
                    pending_quote = 0;
                    field_has_data = 1;
                    continue;
                }
                quoted = 0;
                pending_quote = 0;
            } else if (ch == '"') {
                pending_quote = 1;
                continue;
            } else {
                field_has_data = 1;
                continue;
            }
        }

        if (ch == '"' && !field_has_data) {
            quoted = 1;
            field_has_data = 1;
        } else if (ch == ',') {
            ++stats->fields;
            field_has_data = 0;
        } else if (ch == '\n') {
            ++stats->fields;
            ++stats->records;
            field_has_data = 0;
        } else if (ch != '\r') {
            field_has_data = 1;
        }
    }
}
