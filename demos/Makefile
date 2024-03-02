run:
	$(MAKE) -C address_book
	$(MAKE) -C address_book clean
	$(MAKE) -C hello_world
	$(MAKE) -C hello_world clean
	$(MAKE) -C oneof
	$(MAKE) -C oneof clean

new:
	mkdir -p new_example/.files
	mkdir -p new_example/user_space
	go run ../cmd/command/main.go init new_example/module
