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

## How to use (building from source)
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

## Examples
Inside the [Examples](examples/README.md) folder there are some examples on how this project can be used. In particular there are:

### [UDP Sockets](examples/socket_udp/README.md)
![UDP Socket](docs/img/UDP_socket.png)
Here an UDP socket is opened inside the LKM to receive a protobuf message, that is then decoded and printed to dmesg.

### [Netfilter](examples/netfilter/README.md)
![Netfilter](docs/img/netfilter.png)
Here the module is used to filter tcp packets between two applications and apply some drop logics based on the protobuf content.


## Docs
See the [Docs](docs) folder for docs or build them locally (requires docker installed):
```shell
make docs
```

## Special thanks to
- [eerimoq/pbtools][pbtools] for the library I used for the port
- [gschwaer/vscode-kernel-module][vscode-kernel-module] for the vscode setup
- [phip1611/cmake-kernel-module][cmake-kernel-module] for the clion setup
- [Linux-Kernel-Programming-Part-2][LKP-2] for the module Makefile template
- [linux-kernel-labs][linux-kernel-labs] for the guides on how to use sockets inside the kernel


[protocol-buffers]: https://developers.google.com/protocol-buffers
[proto3]: https://developers.google.com/protocol-buffers/docs/proto3
[pbtools]: https://github.com/eerimoq/pbtools
[vscode-kernel-module]: https://github.com/gschwaer/vscode-kernel-module
[cmake-kernel-module]: https://gitlab.com/phip1611/cmake-kernel-module
[linux-kernel-labs]: https://linux-kernel-labs.github.io/refs/heads/master/labs/networking.html
[LKP-2]: https://github.com/PacktPublishing/Linux-Kernel-Programming-Part-2