# Netfilter

## Hello World

```proto
message Foo {
    int32 bar = 1;
}
```

### How to run
1. Build, Install & Load module
```shell
cd hello_world/module
make
make install
make load
```

2. Start server
```shell
cd hello_world/user_space
make server
```

3. Start client
```shell
cd hello_world/user_space
make client
```

