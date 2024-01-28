# Project Structure

The project is composed by two main pieces:
- Folder Initializer
- Code Generator

# Folder Initializer
Creates new folder from template

## requirements
- golang >= 1.21.6 (to build from source)

## build executable
```shell
make
```

## Usage
```shell
./pbtools_lkm init new_folder_name
```

# Code Generator
Generates `.c` and `.h` file from `.proto` specification

## Requirements
- python3
- python3-venv
- pip3

## Setup environment
```shell
python3 -m venv ../..
env PYTHONPATH=../.. \
   pip3 install textparser
```

## Usage
```shell
env PYTHONPATH=../../.. \
   python3 -m pbtools generate_c_source path/to/file.proto
```