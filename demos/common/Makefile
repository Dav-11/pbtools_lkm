examples := address_book hello_world oneof

all: requirements $(examples)
	@
	@ echo "Generating code for all examples..."
	@

requirements:
	python3 -m venv ./venv
	PYTHONPATH=$(CURDIR) venv/bin/pip3 install textparser

.PHONY: address_book
address_book:
	
	@
	@ echo "clean old files if exists"
	@
	mkdir -p address_book/generated
	rm -f address_book/generated/*.c

	@
	@ echo "Generating..."
	@
	cd address_book/generated && \
		export PYTHONPATH=../.. && \
		../../venv/bin/python3 -m pbtools generate_c_source ../proto/address_book.proto

	@
	@ echo "correcting non-standard pbtools.h location"
	@
	sed -i 's/#include "..\/pbtools.h"/#include "..\/..\/pbtools.h"/' address_book/generated/address_book.h

.PHONY: hello_world
hello_world:
	
	@
	@ echo "clean old files if exists"
	@
	mkdir -p hello_world/generated
	rm -f hello_world/generated/*.c

	@
	@ echo "Generating..."
	@
	cd hello_world/generated && \
		export PYTHONPATH=../.. && \
		../../venv/bin/python3 -m pbtools generate_c_source ../proto/hello_world.proto

	@
	@ echo "correcting non-standard pbtools.h location"
	@
	sed -i 's/#include "..\/pbtools.h"/#include "..\/..\/pbtools.h"/' hello_world/generated/hello_world.h

