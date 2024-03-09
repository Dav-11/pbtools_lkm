package main

import (
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

	rootCmd := &cobra.Command{Use: "pbtools_lkm", Short: "pbtools_lkm - Protobuf LKM"}

	rootCmd.AddCommand(cmdInit)

	if err := rootCmd.Execute(); err != nil {
		panic(err)
	}
}
