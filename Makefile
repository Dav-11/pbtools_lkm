build:
	go build -o pbtools_lkm cmd/init/init.go

test:
	$(MAKE) clean
	go run cmd/init/init.go init test_gen

clean:
	rm -rf test_gen
	rm -f pbtools_lkm
