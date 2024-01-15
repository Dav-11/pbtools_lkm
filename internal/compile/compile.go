package compile

import (
	"fmt"
	"os"

	"github.com/alecthomas/participle/v2"
	"github.com/alecthomas/participle/v2/lexer"
	"github.com/alecthomas/repr"
	"github.com/spf13/cobra"
)

// Compile generates .c and .h files from .proto file
func Compile(cmd *cobra.Command, args []string) {

	var file string
	var parser = participle.MustBuild[Proto](participle.UseLookahead(2))

	switch len(args) {

	case 1:
		file = args[0]

	default:
		if err := cmd.Help(); err != nil {
			panic(err)
		}
		os.Exit(1)
	}

	fmt.Println(file)
	r, err := os.Open(file)
	if err != nil {
		panic(err)
	}

	proto, err := parser.Parse("", r)
	if err != nil {
		panic(err)
	}

	repr.Println(
		proto,
		repr.Hide[lexer.Position](),
	)
}
