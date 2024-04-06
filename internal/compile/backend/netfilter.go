package backend

import (
	"encoding/json"
	"fmt"
	"github.com/yoheimuta/go-protoparser/v4/parser"
)

type NetfilterBackend struct {
}

func (b NetfilterBackend) Output(ir *parser.Proto, _ *string) error {

	gotJSON, err := json.MarshalIndent(ir, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal, err %v\n", err)
	}

	fmt.Print(string(gotJSON))

	return nil
}
