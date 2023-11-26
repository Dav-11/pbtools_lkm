# Understanding what syscall are used
[strace]: https://strace.io/

Running the example/hello_world with [strace][strace]:

```shell
cd examples/hello_world
make generate # (re)compile proto files
make build    # build executable
strace ./main # run executable with strace
```

Output:

```text
execve("./main", ["./main"], 0x7fffd8ad35c0 /* 55 vars */) = 0
brk(NULL)                               = 0x56258bc45000
arch_prctl(0x3001 /* ARCH_??? */, 0x7ffd4d5cc2a0) = -1 EINVAL (Invalid argument)
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f3632000000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=81193, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 81193, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f3631fec000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\202\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2105184, ...}, AT_EMPTY_PATH) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2150256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f3631c00000
mmap(0x7f3631c26000, 1568768, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x26000) = 0x7f3631c26000
mmap(0x7f3631da5000, 348160, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1a5000) = 0x7f3631da5000
mmap(0x7f3631dfa000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1f9000) = 0x7f3631dfa000
mmap(0x7f3631e00000, 53104, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f3631e00000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f3631fe9000
arch_prctl(ARCH_SET_FS, 0x7f3631fe9740) = 0
set_tid_address(0x7f3631fe9a10)         = 6764
set_robust_list(0x7f3631fe9a20, 24)     = 0
rseq(0x7f3631fea060, 0x20, 0, 0x53053053) = 0
mprotect(0x7f3631dfa000, 16384, PROT_READ) = 0
mprotect(0x56258b078000, 4096, PROT_READ) = 0
mprotect(0x7f3632037000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7f3631fec000, 81193)           = 0
newfstatat(1, "", {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}, AT_EMPTY_PATH) = 0
getrandom("\x59\xc5\x69\x55\x46\x3a\x01\x0c", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x56258bc45000
brk(0x56258bc66000)                     = 0x56258bc66000
write(1, "Successfully encoded Foo into 2 "..., 39Successfully encoded Foo into 2 bytes.) = 39
write(1, "Successfully decoded 2 bytes int"..., 39Successfully decoded 2 bytes into Foo.) = 39
write(1, "Foo.bar: 78\n", 12Foo.bar: 78)           = 12
exit_group(0)                           = ?
+++ exited with 0 +++
```

## Analysis

### execve
- https://www.man7.org/linux/man-pages/man2/execve.2.html[man]

Replace the current running program with the one in $1 (./main)

### brk(NULL)
- https://man7.org/linux/man-pages/man2/brk.2.html[man]

Check the current position of _program break_ (check where the current heap ends)

### arch_prctl(0x3001 /* ARCH_??? */, 0x7ffd4d5cc2a0)
- https://www.man7.org/linux/man-pages/man2/arch_prctl.2.html[man]

### mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
- https://www.man7.org/linux/man-pages/man2/mmap.2.html[man]
- https://stackoverflow.com/a/39945292[stackoverflow post]

Alloc 8KB

### access("/etc/ld.so.preload", R_OK)
- https://linux.die.net/man/8/ld.so[ls.so]
- https://superuser.com/questions/1183037/what-is-does-ld-so-preload-do[ld.so.preload]

Checks if we can access the file `/etc/ld.so.preload` (list of libraries to load for linking)

### openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC)
- https://linux.die.net/man/2/openat[man]

Open the file

### newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=81193, ...}, AT_EMPTY_PATH)
Get info on the file

### mmap(NULL, 81193, PROT_READ, MAP_PRIVATE, 3, 0)
Alloc 79KB

### close(3)
Close `/etc/ld.so.preload`

### openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC)
Open `/lib/x86_64-linux-gnu/libc.so.6` file (shared library)

### read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\202\2\0\0\0\0\0"..., 832)
Read the library

### pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64)
Read 784 bytes from file from pos 64

### close(3)
Close `/lib/x86_64-linux-gnu/libc.so.6` file