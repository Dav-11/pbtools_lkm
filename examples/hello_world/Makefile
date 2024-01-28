all:
	@echo
	@echo "--- regen from proto ---"
	@echo
	$(MAKE) -C module generate
	@echo
	@echo "--- install LKM ---"
	@echo
	$(MAKE) -C module install
	@echo
	@echo "--- Load LKM ---"
	@echo
	$(MAKE) -C module load
	@echo
	@echo "--- Build client app ---"
	@echo
	$(MAKE) -C user_space

clean:
	@echo
	@echo "--- clean ---"
	@echo
	$(MAKE) -C module clean
	@echo
	@echo "--- unload ---"
	@echo
	$(MAKE) -C module unload

generate:
	rm -rf module
	go run ../../cmd/init/init.go init module
	cp .files/address_book.proto module/proto/address_book.proto
	cp .files/main.c module/main.c


