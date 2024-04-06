package compile

import (
	"github.com/Dav-11/pbtools_lkm/internal/compile/backend"
	"github.com/Dav-11/pbtools_lkm/internal/compile/frontend"
	"github.com/spf13/cobra"
	"os"
)

// Proto is the path to the protobuf
var Proto string
var Debug bool
var ImportPath string
var OutputFolder string

var fe frontend.Parser
var be backend.Backend

func Run(cmd *cobra.Command, args []string) {

	if len(args) != 1 {

		if err := cmd.Help(); err != nil {
			panic(err)
		}
		os.Exit(1)
	}

	Proto = args[0]

	fe = frontend.SingleFileParser{}

	var importPath *string
	var outputPath *string

	if len(ImportPath) > 0 {
		importPath = &ImportPath
	}

	if len(OutputFolder) > 0 {
		outputPath = &OutputFolder
	}

	ir, err := fe.Parse(Proto, importPath)
	if err != nil {
		panic(err)
	}

	switch OutputTypeF.Value {

	case Json:
		be = backend.JsonBackend{}
	case Netfilter:
		be = backend.NetfilterBackend{}
	default:
		be = backend.JsonBackend{}
	}

	if err := be.Output(ir, outputPath); err != nil {
		panic(err)
	}
}
