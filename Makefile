
test:
	$(MAKE) clean
	go run cmd/command/main.go init test_gen
	$(MAKE) -C test_gen requirements
	$(MAKE) -C test_gen generate

clean:
	rm -rf 	venv

.PHONY: docs
docs:
	docker build -t dav-11/pbtools_lkm/docs:local -f docs/.docker/Dockerfile .
	docker run --rm --name pbtools-lkm-docs -p 9090:80 dav-11/pbtools_lkm:local
