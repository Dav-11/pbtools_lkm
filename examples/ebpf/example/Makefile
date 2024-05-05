all: vmlinux.h example.bpf.o example.skel.h example

vmlinux.h:
	bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

clean:
	rm -f vmlinux.h
	rm -f example.bpf.o
	rm -f example.skel.h
	rm -f example

example.bpf.o:
	clang \
		-target bpf \
		-D __TARGET_ARCH_$(ARCH) \
		-I/usr/include/$(shell uname -m)-linux-gnu \
		-Wall \
		-O2 -g \
		-o example.bpf.o \
		-c example.bpf.c
	llvm-strip -g example.bpf.o


example.skel.h:
	bpftool gen skeleton example.bpf.o > example.skel.h

example:
	gcc \
		-Wall \
		-o example \
		-L/usr/lib64/ \
		-l:libbpf.a -lelf -lz \
		example.c
