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
obj-m 			+= oneof.o

# we have no file "pbtools_lkm.c" in this example
# therefore we specify: module pbtools_lkm.ko relies on
# main.c and pbtools.c ... it's this makefile module magic thing..
# see online resources for more information
# YOU DON'T need this IF you have *.c-file with the name of the final kernel module :)
oneof-y 	:= \
	main.o \
	pbtools.o \
	generated/oneof.o

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
	@echo
	@echo "--- installing ---"
	@echo " [First, invoke the 'make' ]"
	make
	@echo
	@echo " [Now for the 'sudo make install' ]"
	sudo make -C $(KDIR) M=$(PWD) modules_install
	sudo depmod

clean:
	@echo
	@echo "--- cleaning ---"
	@echo
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(PWD)/.vscode/compile_commands.json
	rm -f *~   # from 'indent'

load:
	sudo insmod $(PWD)/oneof.ko

unload:
	sudo rmmod oneof

requirements:
	python3 -m venv ./venv
	PYTHONPATH=$(CURDIR) venv/bin/pip3 install textparser

generate:
	rm -rf generated
	mkdir -p generated
	cd generated && export PYTHONPATH=$(CURDIR) && ../venv/bin/python3 -m pbtools generate_c_source ../proto/oneof.proto

INDENT := indent

# code-style : "wrapper" target over the following kernel code style targets
code-style:
	$(MAKE) indent
	$(MAKE) checkpatch

# indent- "beautifies" C code - to conform to the the Linux kernel
# coding style guidelines.
# Note! original source file(s) is overwritten, so we back it up.
indent:
	@echo
	@echo "--- applying kernel code style indentation with indent ---"
	@echo
	mkdir bkp 2> /dev/null; cp -f *.[chsS] bkp/
	${INDENT} -linux --line-length95 *.[chsS]
	  # add source files as required

# Detailed check on the source code styling / etc
checkpatch:
	$(MAKE) clean
	@echo
	@echo "--- kernel code style check with checkpatch.pl ---"
	@echo
	$(KDIR)/scripts/checkpatch.pl --no-tree -f --max-line-length=95 *.[ch]
	# add source files as required

#--- Static Analysis
# sa : "wrapper" target over the following kernel static analyzer targets
sa:
	$(MAKE) sa_sparse
	$(MAKE) sa_gcc
	$(MAKE) sa_flawfinder
	$(MAKE) sa_cppcheck

# static analysis with sparse
sa_sparse:
	$(MAKE) clean
	@echo
	@echo "--- static analysis with sparse ---"
	@echo
# if you feel it's too much, use C=1 instead
	$(MAKE) C=2 CHECK="/usr/bin/sparse" -C $(KDIR) M=$(PWD) modules

# static analysis with gcc
sa_gcc:
	$(MAKE) clean
	@echo
	@echo "--- static analysis with gcc ---"
	@echo
	$(MAKE) W=1 -C $(KDIR) M=$(PWD) modules

# static analysis with flawfinder
sa_flawfinder:
	$(MAKE) clean
	@echo
	@echo "--- static analysis with flawfinder ---"
	@echo
	flawfinder *.[ch]

# static analysis with cppcheck
sa_cppcheck:
	$(MAKE) clean
	@echo
	@echo "--- static analysis with cppcheck ---"
	@echo
	cppcheck -v --force --enable=all -i .tmp_versions/ -i *.mod.c -i bkp/ --suppress=missingIncludeSystem .

# Packaging; just tar.xz as of now
PKG_NAME := oneof
tarxz-pkg:
	rm -f ../${PKG_NAME}.tar.xz 2>/dev/null
	$(MAKE) clean
	@echo
	@echo "--- packaging ---"
	@echo
	tar caf ../${PKG_NAME}.tar.xz *
	ls -l ../${PKG_NAME}.tar.xz
	@echo '=== package created: ../$(PKG_NAME).tar.xz ==='

help:
	@echo '=== Makefile Help : additional targets available ==='
	@echo
	@echo 'TIP: type make <tab><tab> to show all valid targets'
	@echo

	@echo '--- 'usual' kernel LKM targets ---'
	@echo 'typing "make" or "all" target : builds the kernel module object (the .ko)'
	@echo 'install     : installs the kernel module(s) to INSTALL_MOD_PATH (default here: /lib/modules/$(shell uname -r)/)'
	@echo 'clean       : cleanup - remove all kernel objects, temp files/dirs, etc'

	@echo
	@echo '--- kernel code style targets ---'
	@echo 'code-style : "wrapper" target over the following kernel code style targets'
	@echo ' indent     : run the $(INDENT) utility on source file(s) to indent them as per the kernel code style'
	@echo ' checkpatch : run the kernel code style checker tool on source file(s)'

	@echo
	@echo '--- kernel static analyzer targets ---'
	@echo 'sa         : "wrapper" target over the following kernel static analyzer targets'
	@echo ' sa_sparse     : run the static analysis sparse tool on the source file(s)'
	@echo ' sa_gcc        : run gcc with option -W1 ("Generally useful warnings") on the source file(s)'
	@echo ' sa_flawfinder : run the static analysis flawfinder tool on the source file(s)'
	@echo ' sa_cppcheck   : run the static analysis cppcheck tool on the source file(s)'
	@echo 'TIP: use coccinelle as well (requires spatch): https://www.kernel.org/doc/html/v4.15/dev-tools/coccinelle.html'

	@echo
	@echo '--- kernel dynamic analysis targets ---'
	@echo 'da_kasan   : DUMMY target: this is to remind you to run your code with the dynamic analysis KASAN tool enabled; requires configuring the kernel with CONFIG_KASAN On, rebuild and boot it'
	@echo 'da_lockdep : DUMMY target: this is to remind you to run your code with the dynamic analysis LOCKDEP tool (for deep locking issues analysis) enabled; requires configuring the kernel with CONFIG_PROVE_LOCKING On, rebuild and boot it'
	@echo 'TIP: best to build a debug kernel with several kernel debug config options turned On, boot via it and run all your test cases'

	@echo
	@echo '--- misc targets ---'
	@echo 'tarxz-pkg  : tar and compress the LKM source files as a tar.xz into the dir above; allows one to transfer and build the module on another system'
	@echo 'help       : this help target'
