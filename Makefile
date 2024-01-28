build:
	mkdir -p bin
	CGO_ENABLED=0 go build -o bin/pbtools_lkm -ldflags '-extldflags "-static"' cmd/server/server.go
	GOOS=linux GOARCH=amd64 go build -o bin/pbtools_lkm_linux_amd64 cmd/init/init.go
	GOOS=linux GOARCH=arm64 go build -o bin/pbtools_lkm_linux_arm64 cmd/init/init.go
	GOOS=darwin GOARCH=arm64 go build -o bin/pbtools_lkm_darwin_arm64 cmd/init/init.go
	GOOS=windows GOARCH=amd64 go build -o bin/pbtools_lkm_amd64.exe cmd/init/init.go

test:
	$(MAKE) clean
	go run cmd/init/init.go init test_gen
	$(MAKE) -C test_gen generate

example:
	mkdir -p examples/new_example/.files
	mkdir -p examples/new_example/user_space
	go run cmd/init/init.go init examples/new_example/module

clean:
	rm -rf 	test_gen
	rm -rf 	lib
	rm -rf 	lib64
	rm -rf 	include
	rm -f 	pyvenv.cfg

.PHONY: docs
docs:
	docker build -t Dav-11/pbtools_lkm:local -f docs/.docker/Dockerfile .
	docker run --rm --name pbtools-lkm-docs -p 9090:80 Dav-11/pbtools_lkm:local
