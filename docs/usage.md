# Usage

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

## Create folder
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

### Common errors

#### Skipping BTF generation xxx. due to unavailability of vmlinux
Fix on Ubuntu ([source](https://askubuntu.com/questions/1348250/skipping-btf-generation-xxx-due-to-unavailability-of-vmlinux-on-ubuntu-21-04)):

```
apt install dwarves
cp /sys/kernel/btf/vmlinux /usr/lib/modules/`uname -r`/build/
```

## Unload and clean
```
make unload
make clean
```
