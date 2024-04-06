package frontend

import "github.com/yoheimuta/go-protoparser/v4/parser"

type Parser interface {
	Parse(MainProtoPath string, ImportPath *string) (*parser.Proto, error)
}
