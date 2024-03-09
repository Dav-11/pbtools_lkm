package initialize

import (
	_ "embed"
	"fmt"
	"github.com/spf13/cobra"
	"os"
	"path/filepath"
)

var (
	//go:embed module_template/README.md
	readme string

	//go:embed module_template/pbtools.c
	pbtoolsC string

	//go:embed module_template/pbtools.h
	pbtoolsH string

	//go:embed module_template/Makefile
	makefile string

	//go:embed module_template/main.c
	mainC string

	//go:embed module_template/.gitignore
	gitignore string

	//go:embed module_template/proto/hello_world.proto
	helloWorldProto string

	//
	// python module
	//

	//go:embed module_template/pbtools/version.py
	versionPY string

	//go:embed module_template/pbtools/parser.py
	parserPY string

	//go:embed module_template/pbtools/errors.py
	errorsPY string

	//go:embed module_template/pbtools/__main__.py
	mainPY string

	//go:embed module_template/pbtools/__init__.py
	initPY string

	//go:embed module_template/pbtools/c_source/__init__.py
	cSourceInitPY string

	//go:embed module_template/pbtools/subparsers/__init__.py
	subparsersInitPY string

	//go:embed module_template/pbtools/subparsers/generate_c_source.py
	generateCSourcePY string
)

type Folder struct {
	name      string
	files     []File
	subFolder []Folder
}

func (f *Folder) Write(base string) error {

	// create current path
	folderPath := fmt.Sprintf("%s/%s", base, f.name)
	if err := os.Mkdir(folderPath, os.ModePerm); err != nil {
		return err
	}

	// create files in folder
	for _, file := range f.files {

		if err := file.WriteTo(folderPath); err != nil {
			return err
		}
	}

	// create subfolders
	for _, folder := range f.subFolder {

		if err := folder.Write(folderPath); err != nil {
			return err
		}
	}

	return nil
}

type File struct {
	fileName string
	content  string
}

func (f *File) WriteTo(directory string) error {

	confPath := filepath.Join(directory, f.fileName)
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	if _, err = confFile.WriteString(f.content); err != nil {
		return err
	}

	return nil
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

	folder := Folder{
		files: []File{
			{
				fileName: "README.md",
				content:  readme,
			},
			{
				fileName: "pbtools.c",
				content:  pbtoolsC,
			},
			{
				fileName: "pbtools.h",
				content:  pbtoolsH,
			},
			{
				fileName: "Makefile",
				content:  makefile,
			},
			{
				fileName: "main.c",
				content:  mainC,
			},
			{
				fileName: ".gitignore",
				content:  gitignore,
			},
		},
		subFolder: []Folder{
			{
				name: "proto",
				files: []File{
					{
						fileName: "hello_world.proto",
						content:  helloWorldProto,
					},
				},
			},
			{
				name: "pbtools",
				files: []File{
					{
						fileName: "version.py",
						content:  versionPY,
					},
					{
						fileName: "parser.py",
						content:  parserPY,
					},
					{
						fileName: "errors.py",
						content:  errorsPY,
					},
					{
						fileName: "__main__.py",
						content:  mainPY,
					},
					{
						fileName: "__init__.py",
						content:  initPY,
					},
				},
				subFolder: []Folder{
					{
						name: "c_source",
						files: []File{
							{
								fileName: "__init__.py",
								content:  cSourceInitPY,
							},
						},
					},
					{
						name: "subparsers",
						files: []File{
							{
								fileName: "__init__.py",
								content:  subparsersInitPY,
							},
							{
								fileName: "generate_c_source.py",
								content:  generateCSourcePY,
							},
						},
					},
				},
			},
		},
	}

	// create files in folder
	for _, file := range folder.files {

		if err := file.WriteTo(directory); err != nil {
			panic(err)
		}
	}

	// create subfolders
	for _, folder := range folder.subFolder {

		if err := folder.Write(directory); err != nil {
			panic(err)
		}
	}
}
