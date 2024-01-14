package main

import (
	"github.com/spf13/cobra"
	"github.com/Dav-11/pbtools_lkm/internal/initialize"
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

	if err := initialize.WriteReadme(directory); err != nil {
		panic(err)
	}
	if err := initialize.WritePBToolsC(directory); err != nil {
		panic(err)
	}
	if err := initialize.WritePBToolsH(directory); err != nil {
		panic(err)
	}
	if err := initialize.WriteMakeFile(directory); err != nil {
		panic(err)
	}
	if err := initialize.WriteMainC(directory); err != nil {
		panic(err)
	}
	if err := initialize.CreateProtoFolder(directory); err != nil {
		panic(err)
	}
	if err := initialize.WriteGitignore(directory); err != nil {
		panic(err)
	}
	if err := initialize.WriteCMakeList(directory); err != nil {
		panic(err)
	}
	if err := initialize.CreateCmakeFolder(directory); err != nil {
		panic(err)
	}
	if err := initialize.CreateVSFolder(directory); err != nil {
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
