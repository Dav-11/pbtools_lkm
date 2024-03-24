TARGET = skb_example
ARCH = $(shell uname -m | sed 's/x86_64/x86/' | sed 's/aarch64/arm64/')

BPF_OBJ = ${TARGET:=.bpf.o}

.PHONY: all
.PHONY: $(TARGET)

all:
	$(MAKE) build

build:
	clang \
		-target bpf \
		-D __BPF_TRACING__ \
		-I/usr/include/$(shell uname -m)-linux-gnu \
		-Wall \
		-O2 \
		-o $(BPF_OBJ) -c \
		skb_example.bpf.c

load:
	# Load your eBPF program into the kernel
	bpftool prog load $(BPF_OBJ) /sys/fs/bpf/$(TARGET)

	# Attach the eBPF program to a socket
	bpftool prog attach pinned /sys/fs/bpf/$(TARGET) type sk_skb

unload:


$(TARGET): $(BPF_OBJ)
	bpftool net detach xdp dev lo
	rm -f /sys/fs/bpf/$(TARGET)
	bpftool prog load $(BPF_OBJ) /sys/fs/bpf/$(TARGET)
	bpftool net attach xdp pinned /sys/fs/bpf/$(TARGET) dev lo

requirements:
	apt install -y clang llvm gcc libbpf1 libbpf-dev libbpf-tools libxdp-dev libxdp1 xdp-tools linux-tools-common