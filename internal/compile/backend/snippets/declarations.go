package snippets

const MessageDeclaration = `/**
 * Encoding and decoding of {{ .FullName }}.
 */
struct {{ .FullNameSnakeCase }}_t *
{{ FullNameSnakeCase }}_new(
    void *workspace_p,
    size_t size);

int {{ .FullNameSnakeCase }}_encode(
    struct {{ .FullNameSnakeCase }}_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int {{ .FullNameSnakeCase }}_decode(
    struct {{ .FullNameSnakeCase }}_t *self_p,
    const uint8_t *encoded_p,
    size_t size);`

const RepeatedDeclaration = `
int {{ .MessageFullNameSnakeCase }}_{{ .FieldNameSnakeCase }}_alloc(
    struct {{ .MessageFullNameSnakeCase }}_t *self_p,
    int length);`

const SubMessageDeclaration = `
int {{ .MessageFullNameSnakeCase }}_{{ .FieldNameSnakeCase }}_alloc(
    struct {{ .MessageFullNameSnakeCase }}_t *self_p);`
