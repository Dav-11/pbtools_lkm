package initialize

import (
	_ "embed"
	"fmt"
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

	//go:embed module_template/CMakeLists.txt
	cMakeLists string

	//go:embed module_template/cmake/FindKernelHeaders.cmake
	findKernelHeaders string

	//go:embed module_template/.vscode_t/c_cpp_properties.json
	cCppProperties string

	//go:embed module_template/.vscode_t/module_hello.code-workspace
	moduleHello string
)

// CreateVSFolder creates .vscode folder
func CreateVSFolder(directory string) error {

	vsFolder := fmt.Sprintf("%s/.vscode", directory)

	if err := os.Mkdir(vsFolder, os.ModePerm); err != nil {
		return err
	}

	if err := WriteModuleHello(vsFolder); err != nil {
		return err
	}

	if err := WriteCCppProperties(vsFolder); err != nil {
		return err
	}

	return nil
}

func WriteModuleHello(directory string) error {

	confPath := filepath.Join(directory, "module_hello.code-workspace")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(moduleHello)
	if err != nil {
		return err
	}

	return nil
}

func WriteCCppProperties(directory string) error {

	confPath := filepath.Join(directory, "c_cpp_properties.json")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(cCppProperties)
	if err != nil {
		return err
	}

	return nil
}

// CreateCmakeFolder creates cmake folder
func CreateCmakeFolder(directory string) error {

	cmake := fmt.Sprintf("%s/cmake", directory)

	if err := os.Mkdir(cmake, os.ModePerm); err != nil {
		return err
	}

	if err := WriteFindKernelHeaders(cmake); err != nil {
		return err
	}

	return nil
}

func WriteFindKernelHeaders(directory string) error {

	confPath := filepath.Join(directory, "FindKernelHeaders.cmake")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(findKernelHeaders)
	if err != nil {
		return err
	}

	return nil
}

func WriteCMakeList(directory string) error {

	confPath := filepath.Join(directory, "CMakeLists.txt")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(cMakeLists)
	if err != nil {
		return err
	}

	return nil
}

// WriteReadme writes the README.md file
func WriteReadme(directory string) error {

	confPath := filepath.Join(directory, "README.md")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(readme)
	if err != nil {
		return err
	}

	return nil
}

// WritePBToolsC writes the pbtools.c file
func WritePBToolsC(directory string) error {

	confPath := filepath.Join(directory, "pbtools.c")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(pbtoolsC)
	if err != nil {
		return err
	}

	return nil
}

// WritePBToolsH writes the pbtools.h file
func WritePBToolsH(directory string) error {

	confPath := filepath.Join(directory, "pbtools.h")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(pbtoolsH)
	if err != nil {
		return err
	}

	return nil
}

// WriteMakeFile writes the Makefile file
func WriteMakeFile(directory string) error {

	confPath := filepath.Join(directory, "Makefile")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(makefile)
	if err != nil {
		return err
	}

	return nil
}

// WriteMainC writes the main.c file
func WriteMainC(directory string) error {

	confPath := filepath.Join(directory, "main.c")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(mainC)
	if err != nil {
		return err
	}

	return nil
}

// WriteGitignore writes the .gitignore file
func WriteGitignore(directory string) error {

	confPath := filepath.Join(directory, ".gitignore")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	_, err = confFile.WriteString(gitignore)
	if err != nil {
		return err
	}

	return nil
}

// CreateProtoFolder creates the proto folder
func CreateProtoFolder(directory string) error {

	protoFolder := fmt.Sprintf("%s/proto", directory)

	if err := os.Mkdir(protoFolder, os.ModePerm); err != nil {
		return err
	}

	if err := writeExampleProto(protoFolder); err != nil {
		return err
	}

	return nil
}

// writeExampleProto writes hello_world.proto
func writeExampleProto(directory string) error {

	confPath := filepath.Join(directory, "hello_world.proto")
	confFile, err := os.OpenFile(confPath, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0o666)
	if err != nil {
		return err
	}
	defer confFile.Close()

	if _, err = confFile.WriteString(helloWorldProto); err != nil {
		return err
	}

	return nil
}
