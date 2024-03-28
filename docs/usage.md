# Usage

Here it is explained how to initialize, compile and load a new module

## Requirements
- golang >= 1.21.6
- python3
- python3-env
- python3-pip
- make
- gcc
- linux os (x LKM)

### Build executable
```
make
```

## Init module (creates new folder)
```
bin/pbtools_lkm init <folder_name>
cd <folder_name>
```

## Edit proto files
Delete `<folder_name>/proto/hello_world.proto` and write new proto files

## Compile
```
cd <folder_name>
make requirements
make generate
```

## Edit main
Edit `main.c` to implement functions

## Compile and load module
```
cd <folder_name>
make build
make install
make load
```

## Unload and clean
```
make unload
make clean
```
