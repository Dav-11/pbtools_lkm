# Build

1. Copy the [module_template]() folder.
2. Add your `.proto` files inside the `proto` folder.
3. Edit the Makefile file's generate target with the right `.proto` file name.
4. Run this command to generate `.c` and `.h` files from proto inside the `generated` folder.
    ```shell
    make generate
    ```
5. Edit the code in `main.c` as needed.
6. Update the `pbtools_lkm-y` variable inside the Makefile file with the generated file name.
7. Build the module.
    ```shell
    make build
    ```
8. Install and load the module.
    ```shell
    make install && make load
    ```

