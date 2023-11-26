all:
	$(MAKE) generate
	$(MAKE) build
	$(MAKE) run

generate:
	rm -rf generated
	mkdir -p generated
	cd generated && \
	    env PYTHONPATH=../../.. \
		python3 -m pbtools generate_c_source ../fie_bar.proto

build:
	gcc \
		-O2 \
		-Wall \
		-Wno-gnu-alignof-expression \
		-I ../../lib/include \
		-Igenerated \
		generated/*.c \
		../../lib/src/pbtools.c \
		main.c \
		-o main

run:
	./main
