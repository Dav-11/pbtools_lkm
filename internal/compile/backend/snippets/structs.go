package snippets

const MessageStruct = `/**
 * Message {{ .Name }}
 */

{repeated_struct}

struct {{ .FullNameSnakeCase }}_t {
    struct pbtools_message_base_t base;
{{range .Members}}
	{{.}}
{{end}}
};`

const RepeatedMessageStruct = `
struct {{ .FullNameSnakeCase }}_repeated_t {
    int length;
    struct {{ .FullNameSnakeCase }}_t *items_p;
};`

const Enum = `/**
 * Enum {{ .FullName }}
 */
enum {{ .NameSnakeCase }}_e {{
{{range .Members}}
	{{ .NamespaceSnakeCase }}_{{ .NameSnakeCase }}_e = {{.FieldNumber}}
{{end}}
}};`

const OneOf = `
    enum {.FullNameSnakeCase}_e {.NameSnakeCase};
    union {
{{range .Members}}
		{{.}}
{{ end }}
    };`
