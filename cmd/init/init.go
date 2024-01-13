package main

import (
	"init/internal/generate"
	"github.com/spf13/cobra"
	"os"
)

func main() {
	
	cmdInit := &cobra.Command{
		Use:   "init DIRECTORY",
		Short: "Initialize a new project",
		Long:  "Initialize a new project in DIRECTORY",
		Run:   Init,
	}

	cmdCompile := &cobra.Command{
		Use:   "generate_c_source file.proto",
		Short: "Generates .c and .h files from .proto file",
		Long:  "Generates .c and .h files from .proto file",
		Run:   Compile,
	}

	rootCmd := &cobra.Command{Use: "pbtools_lkm", Short: "pbtools_lkm - Protobuf LKM"}
	
	rootCmd.AddCommand(cmdInit)
	rootCmd.AddCommand(cmdCompile)
	
	if err := rootCmd.Execute(); err != nil {
		panic(err)
	}
}

// Init creates an example folder from template
func Init(cmd *cobra.Command, args []string) {

	var directory string

	switch len(args) {
	case 0:
		directory = "."
	case 1:
		directory = args[0]
		if err := os.Mkdir(directory, os.ModePerm); err != nil {
			panic(err)
		}
	default:
		if err := cmd.Help(); err != nil {
			panic(err)
		}
		os.Exit(1)
	}

	if err := generate.WriteReadme(directory); err != nil {
		panic(err)
	}
	if err := generate.WritePBToolsC(directory); err != nil {
		panic(err)
	}
	if err := generate.WritePBToolsH(directory); err != nil {
		panic(err)
	}
	if err := generate.WriteMakeFile(directory); err != nil {
		panic(err)
	}
	if err := generate.WriteMainC(directory); err != nil {
		panic(err)
	}
	if err := generate.CreateProtoFolder(directory); err != nil {
		panic(err)
	}
	if err := generate.WriteGitignore(directory); err != nil {
		panic(err)
	}
	if err := generate.WriteCMakeList(directory); err != nil {
		panic(err)
	}
	if err := generate.CreateCmakeFolder(directory); err != nil {
		panic(err)
	}
	if err := generate.CreateVSFolder(directory); err != nil {
		panic(err)
	}
}

// Compile generates .c and .h files from .proto file
func Compile(cmd *cobra.Command, args []string) {

	var directory string

	switch len(args) {
		case 1:
			directory = args[0]
			if err := os.Mkdir(directory, os.ModePerm); err != nil {
				panic(err)
			}

		default:
			if err := cmd.Help(); err != nil {
				panic(err)
			}
			os.Exit(1)
	}
}
