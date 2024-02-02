build:
	mkdir -p bin
	CGO_ENABLED=0 go build -o bin/pbtools_lkm -ldflags '-extldflags "-static"' cmd/command/main.go
	GOOS=linux GOARCH=amd64 go build -o bin/pbtools_lkm_linux_amd64 cmd/command/main.go
	GOOS=linux GOARCH=arm64 go build -o bin/pbtools_lkm_linux_arm64 cmd/command/main.go
	GOOS=darwin GOARCH=arm64 go build -o bin/pbtools_lkm_darwin_arm64 cmd/command/main.go
	GOOS=windows GOARCH=amd64 go build -o bin/pbtools_lkm_amd64.exe cmd/command/main.go

test:
	$(MAKE) clean
	go run cmd/command/main.go init test_gen
	$(MAKE) -C test_gen requirements
	$(MAKE) -C test_gen generate

.PHONY: examples
examples:
	$(MAKE) -C examples generate
	$(MAKE) -C examples run

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
