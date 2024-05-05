# eBPF + kfunc
> This section is a WIP and does not currently work !

To use this, pull also submodules
```shell
git submodule update --init --recursive
```

## ENV
- linux kernel v6.7.9
```shell
root@kernel:~# uname -r
6.7.9
```
- bpftool
```shell
root@kernel:~# bpftool version
bpftool v7.3.0
using libbpf v1.3
features: llvm, skeletons
```