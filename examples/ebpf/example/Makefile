INCLUDE_DIR := ../include
LIBBPF_OBJ := $(abspath $(INCLUDE_DIR)/libbpf.a)
VMLINUX_H := $(abspath $(INCLUDE_DIR)/vmlinux.h)

ifeq ($(V),1)
	Q =
	msg =
else
	Q = @
	msg = @printf '  %-8s %s%s\n'							\
		      "$(1)"										\
		      "$(patsubst $(abspath $(OUTPUT))/%,%,$(2))"	\
		      "$(if $(3), $(3))";
	MAKEFLAGS += --no-print-directory
endif

all: $(VMLINUX_H) $(LIBBPF_OBJ) example.bpf.o example.skel.h example

$(VMLINUX_H):
	$(call msg,VMLINUX,$@)
	$(Q) bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

$(LIBBPF_OBJ):
	$(Q) $(MAKE) -C ../deps/libbpf/src/ 			\
		BUILD_STATIC_ONLY=1 						\
		OBJDIR=$(dir $@)/libbpf DESTDIR=$(dir $@) 	\
		INCLUDEDIR= LIBDIR= UAPIDIR= 				\
		install

bpftool:
	$(Q) $(MAKE) -C ../deps/bpftool/src install

clean:
	rm -f vmlinux.h
	rm -f example.bpf.o
	rm -f example.skel.h
	rm -f example
	rm -rf $(INCLUDE_DIR)

example.bpf.o:
	$(call msg,BPF,$@)
	$(Q) clang 										\
		-target bpf 								\
		-D __TARGET_ARCH_$(ARCH) 					\
		-I/usr/include/$(shell uname -m)-linux-gnu 	\
		-I$(INCLUDE_DIR) 							\
		-Wall 										\
		-O2 -g 										\
		-o example.bpf.o 							\
		-c example.bpf.c
	$(Q) llvm-strip -g example.bpf.o


example.skel.h:
	$(call msg,GEN-SKEL,$@)
	$(Q) bpftool gen skeleton example.bpf.o > example.skel.h

example:
	$(call msg,BINARY,$@)
	$(Q)$(CC) 				\
		-Wall 				\
		-o example 			\
		-lelf -lz 			\
		-I$(INCLUDE_DIR) 	\
		example.c

