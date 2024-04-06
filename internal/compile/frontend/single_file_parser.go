package frontend

import (
	"fmt"
	"github.com/yoheimuta/go-protoparser/v4"
	"github.com/yoheimuta/go-protoparser/v4/parser"
	"os"
	"path/filepath"
)

type SingleFileParser struct {
	Debug      bool
	Permissive bool
}

func (s SingleFileParser) Parse(MainProtoPath string, _ *string) (*parser.Proto, error) {

	// open file
	reader, err := os.Open(MainProtoPath)
	if err != nil {
		return nil, fmt.Errorf("failed to open %s, err %v\n", MainProtoPath, err)
	}

	defer func() {
		if err := reader.Close(); err != nil {
			fmt.Printf("Error closing file: %v\n", err)
		}
	}()

	// parse
	got, err := protoparser.Parse(
		reader,
		protoparser.WithDebug(s.Debug),
		protoparser.WithPermissive(s.Permissive),
		protoparser.WithFilename(filepath.Base(MainProtoPath)),
	)
	if err != nil {
		return nil, fmt.Errorf("failed to parse, err %v\n", err)
	}

	return got, nil
}
