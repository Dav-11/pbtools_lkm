# pbtools_lkm

[Google Protocol Buffers][protocol-buffers] implementation that runs as a linux kernel module.

> This is a university project, **DO NOT USE IT IN PROD** as is.

The project is based (and is a fork of) [eerimoq/pbtools][pbtools] and shares its limitations:

- Options, services (gRPC) and reserved fields are ignored.
- Public imports are not implemented.

## Requirements
- golang >= 1.21.6
- python3
- python3-env
- python3-pip
- make
- gcc
- Linux OS

## How to use (build from source)
> All commands are meant to be run from the root folder of this project

### Build executable
```shell
make
```

### Initialize a new folder for your project 
```shell
bin/pbtools_lkm init <folder_name>
cd <folder_name>
```

### Edit proto files
Delete `<folder_name>/proto/hello_world.proto` and write new proto files

### Generate the C code from the protobuf specification
```shell
cd <folder_name>
make requirements
make generate
```

### Edit main
Edit `<folder_name>/main.c` to implement the desired logic

### Compile and load module
```shell
cd <folder_name>
make build
make install
make load
```

### Unload and clean
To delete the kernel module from the system and clean the folder run
```shell
cd <folder_name>
make unload
make clean
```

## [Demos](./demos/README.md)
This folder contains some examples of how this project can be used.

### Socket UDP



## Docs
See the [Docs](docs) folder for docs or build them locally (requires docker installed):
```shell
make docs
```


## Special thanks to
- https://github.com/eerimoq/pbtools for the library I used for the port
- https://github.com/gschwaer/vscode-kernel-module for the vscode setup
- https://gitlab.com/phip1611/cmake-kernel-module for the clion setup
- https://github.com/PacktPublishing/Linux-Kernel-Programming-Part-2/tree/main
- https://linux-kernel-labs.github.io/refs/heads/master/labs/networking.html


[protocol-buffers]: https://developers.google.com/protocol-buffers
[proto3]: https://developers.google.com/protocol-buffers/docs/proto3
[pbtools]: https://github.com/eerimoq/pbtools