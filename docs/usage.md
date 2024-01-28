# Usage

## Requirements
- golang >= 1.21.6
- python3
- python3-env
- python3-pip

### Build executable
```shell
make
```

## Create folder
```shell
bin/pbtools_lkm init <folder_name>
cd <folder_name>
```

## Edit proto files
Delete `<folder_name>/proto/hello_world.proto` and write new proto files

## Compile
```shell
cd <folder_name>
make requirements
make generate
```

## (optional) Add generated files to `CMakeLists.txt`
```
add_executable(dummy
        # add all *.h and *.c files here that CLion should cover
        main.c
        pbtools.c
)
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
```shell
make unload
make clean
```
