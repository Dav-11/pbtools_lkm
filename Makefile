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

example:
	go run cmd/init/init.go init examples/new_example

clean:
	rm -rf 	test_gen
	rm -rf 	lib
	rm -rf 	lib64
	rm -rf 	include
	rm -f 	pyvenv.cfg
