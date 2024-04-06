package cmd

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"
)

var rootCmd = &cobra.Command{
	Use:   "pbtools_lkm",
	Short: "protobuf LKM generator",
	Long:  `pbtools_lkm is a template engine to generate a kernel module stub with support to encode/decode a protobuf message`,
}

func init() {
	rootCmd.AddCommand(cmdInit)
	rootCmd.AddCommand(cmdCompile)
}

// Execute root command
func Execute() {

	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
