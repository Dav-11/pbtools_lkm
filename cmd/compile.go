package cmd

import (
	"fmt"
	"github.com/Dav-11/pbtools_lkm/internal/compile"
	"github.com/spf13/cobra"
)

var cmdCompile = &cobra.Command{
	Use:   "compile path/to/file.proto",
	Short: "Creates C code for specified protobuf file",
	Long:  `Generates <your-proto>.h and <your-proto>.c`,
	Args:  cobra.MinimumNArgs(1),
	Run:   compile.Run,
}

func init() {
	cmdCompile.Flags().StringVarP(&compile.OutputFolder, "output-directory", "o", ".", "path to the output folder")
	//cmdCompile.Flags().StringVarP(&compile.ImportPath, "import-path", "I", "", "Path where to search for imports")

	usageString := fmt.Sprintf("Choose an output type. Valid values are: %v.", compile.OutputTypeF.Allowed)
	cmdCompile.Flags().VarP(&compile.OutputTypeF, "output-type", "t", usageString)

	compile.Debug = *cmdCompile.Flags().BoolP("debug", "d", false, "debug flag to output more parsing process detail")
}
