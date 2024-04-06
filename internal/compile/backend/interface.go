package backend

import "github.com/yoheimuta/go-protoparser/v4/parser"

type Backend interface {
	Output(ir *parser.Proto, outputPath *string) error
}
