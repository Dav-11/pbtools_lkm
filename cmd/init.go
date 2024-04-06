package cmd

import (
	"github.com/Dav-11/pbtools_lkm/internal/initialize"
	"github.com/spf13/cobra"
)

var cmdInit = &cobra.Command{
	Use:   "init DIRECTORY",
	Short: "Initialize a new project",
	Long:  "Initialize a new project in DIRECTORY",
	Run:   initialize.Init,
}
