run:
	$(MAKE) -C address_book
	$(MAKE) -C hello_world

generate:
	$(MAKE) -C address_book generate
	$(MAKE) -C hello_world generate

new:
	mkdir -p new_example/.files
	mkdir -p new_example/user_space
	go run ../cmd/command/main.go init new_example/module
