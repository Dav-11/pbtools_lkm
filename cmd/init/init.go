package main

import (
	"github.com/Dav-11/pbtools_lkm/internal/compile"
	"github.com/Dav-11/pbtools_lkm/internal/initialize"
	"github.com/spf13/cobra"
)

func main() {

	cmdInit := &cobra.Command{
		Use:   "init DIRECTORY",
		Short: "Initialize a new project",
		Long:  "Initialize a new project in DIRECTORY",
		Run:   initialize.Init,
	}

	cmdCompile := &cobra.Command{
		Use:   "generate_c_source FILE",
		Short: "Generates .c and .h files from .proto file",
		Long:  "Generates .c and .h files from .proto file",
		Run:   compile.Compile,
	}

	rootCmd := &cobra.Command{Use: "pbtools_lkm", Short: "pbtools_lkm - Protobuf LKM"}

	rootCmd.AddCommand(cmdInit)
	rootCmd.AddCommand(cmdCompile)

	if err := rootCmd.Execute(); err != nil {
		panic(err)
	}
}
