all:
	$(MAKE) build
	./main

build:
	gcc \
		-O2 \
		-Wall \
		-Wno-gnu-alignof-expression \
		main.c \
		pbtools.c \
		generated/hello_world.c \
		-o main
