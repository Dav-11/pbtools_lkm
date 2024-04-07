# Example Usage
Here it is explained how to initialize, compile and load a new module

## Requirements
- python3
- python3-env
- python3-pip
- make
- gcc
- Linux os

## LKM Stub
In this example we use the simple proto-file [hello_world.proto](examples/common/hello_world/proto/hello_world.proto)

```proto
syntax = "proto3";

package hello_world;

message Foo {
    int32 bar = 1;
}
```
1. Create venv
    ```shell
    make requirements
    ```
2. Generate LKM stub from the proto-file. Replace `<module-type>` and `<output-folder>` with your choices.
    ```shell
    export PYTHONPATH=$(pwd)
    ./.venv/bin/python3 -m pbtools_lkm generate_lkm_source -T <module-type> -o <output-folder> examples/common/hello_world/proto/hello_world.proto
    ```

## Compile the module
1. Enter the created folder
    ```shell
    cd <output-folder>
    ```
2. Open `main.c` and add your code inside the function where you find this comment
   ```c
    /*
     * TODO: Place your code here
     */
   ```
3. Compile, install and load the module
    ```shell
    make
    ```
4. See the logs
   ```shell
   dmesg
   ```
5. (optional) Unload the module
   ```shell
   make unload
   ```
