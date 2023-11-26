# To support cross-compiling for kernel modules:
# For architecture (cpu) 'arch', invoke make as:
#  make ARCH=<arch> CROSS_COMPILE=<cross-compiler-prefix>
ifeq ($(ARCH),arm)
  # *UPDATE* 'KDIR' below to point to the ARM Linux kernel source tree on your box
  KDIR ?= ~/rpi_work/kernel_rpi/linux
else ifeq ($(ARCH),arm64)
  # *UPDATE* 'KDIR' below to point to the ARM64 (Aarch64) Linux kernel source
  # tree on your box
  KDIR ?= ~/kernel/linux-4.14
else ifeq ($(ARCH),powerpc)
  # *UPDATE* 'KDIR' below to point to the PPC64 Linux kernel source tree on your box
  KDIR ?= ~/kernel/linux-4.9.1
else
  # 'KDIR' is the Linux 'kernel headers' package on your host system; this is
  # usually an x86_64, but could be anything, really (f.e. building directly
  # on a Raspberry Pi implies that it's the host)
  KDIR ?= /lib/modules/$(shell uname -r)/build
endif


PWD   			:= $(shell pwd)
EXTRA_CFLAGS   	+= -DDEBUG
obj-m 			+= pbtools_lkm.o

# we have no file "pbtools_lkm.c" in this example
# therefore we specify: module pbtools_lkm.ko relies on
# main.c and pbtools.c ... it's this makefile module magic thing..
# see online resources for more information
# YOU DON'T need this IF you have *.c-file with the name of the final kernel module :)
pbtools_lkm-y 	:= \
	main.o \
	pbtools.o \
	generated/benchmark.o

build:

	@echo
	@echo '--- Building : KDIR=${KDIR} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} EXTRA_CFLAGS=${EXTRA_CFLAGS} ---'
	@echo

	# build
	$(MAKE) -C $(KDIR) M=$(PWD) modules

	# vscode fix
	$(KDIR)/scripts/clang-tools/gen_compile_commands.py --directory $(PWD) --output $(PWD)/.vscode/compile_commands.json

	# fix include paths (they are relative to the kernel dir)
	sed -i "s|-I\./|-I$(KDIR)/|g" $(PWD)/.vscode/compile_commands.json
	sed -i "s| \./| $(KDIR)/|g" $(PWD)/.vscode/compile_commands.json
	sed -i "s|-Iubuntu/|-I$(KDIR)/ubuntu/|g" $(PWD)/.vscode/compile_commands.json

install:
	sudo $(MAKE) -C $(KDIR) M=$(PWD) modules_install
	sudo depmod -A

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(PWD)/.vscode/compile_commands.json
	rm -f *~   # from 'indent'

load:
	sudo insmod $(PWD)/pbtools_lkm.ko action=decode

unload:
	sudo rmmod pbtools_lkm

generate:
	rm -rf generated
	mkdir -p generated
	cd generated && \
	    env PYTHONPATH=../../.. \
		python3 -m pbtools generate_c_source ../proto/benchmark.proto
