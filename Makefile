build:
	mkdir -p bin
	CGO_ENABLED=0 go build -o bin/pbtools_lkm -ldflags '-extldflags "-static"' cmd/command/main.go

test:
	$(MAKE) clean
	go run cmd/command/main.go init test_gen
	$(MAKE) -C test_gen requirements
	$(MAKE) -C test_gen generate

clean:
	rm -rf 	test_gen
	rm -rf 	lib
	rm -rf 	lib64
	rm -rf 	include
	rm -f 	pyvenv.cfg
	rm -rf 	bin

.PHONY: docs
docs:
	docker build -t dav-11/pbtools_lkm:local -f docs/.docker/Dockerfile .
	docker run --rm --name pbtools-lkm-docs -p 9090:80 dav-11/pbtools_lkm:local
