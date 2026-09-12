# CSV scanning

Forge starts CSV support with a streaming scanner rather than a row-oriented in-memory representation.

`forge_csv_scan` walks a `FILE *` once and reports the number of records and fields seen. It understands the CSV cases that matter for the next parser layer:

- comma-separated fields
- CRLF and LF line endings
- quoted fields containing commas
- quoted fields containing newlines
- doubled quotes inside quoted fields
- empty trailing fields

The scanner intentionally does not allocate per row. The byte buffer can be used by a future parser when actual field values need to be retained.

This keeps the first CSV layer small and testable while leaving room for a column-oriented ingestion path later.
