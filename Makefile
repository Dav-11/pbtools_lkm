all: venv test

test: venv
	export PYTHONPATH=$(shell pwd)
	./venv/bin/python3 -m pbtools_lkm generate_lkm_source -T udp_socket -o test_gen_udp examples/common/address_book/proto/address_book.proto
	./venv/bin/python3 -m pbtools_lkm generate_lkm_source -T netfilter -o test_gen_nf examples/common/address_book/proto/address_book.proto

venv:
	python3 -m venv ./venv
	export PYTHONPATH=$(shell pwd)
	./venv/bin/pip3 install textparser

clean:
	rm -rf venv
	rm -rf test_gen_udp
	rm -rf test_gen_nf

	$(MAKE) -C examples/socket_udp/address_book/module clean
	$(MAKE) -C examples/socket_udp/hello_world/module clean

	$(MAKE) -C examples/netfilter/address_book/module clean
	$(MAKE) -C examples/netfilter/floats/module clean
	$(MAKE) -C examples/netfilter/hello_world/module clean

.PHONY: docs
docs:
	docker build -t dav-11/pbtools_lkm/docs:local -f docs/.docker/Dockerfile .
	docker run --rm --name pbtools-lkm-docs -p 9090:80 dav-11/pbtools_lkm:local
