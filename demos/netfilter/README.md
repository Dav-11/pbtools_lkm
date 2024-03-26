# Netfilter

## Hello World
This hook drops all packages where bar > 50

### Proto
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

## Address Book

### Proto
```proto
syntax = "proto3";

package address_book;

enum PhoneType {
    MOBILE = 0;
    HOME = 1;
    WORK = 2;
}

message PhoneNumber {
    string number = 1;
    PhoneType type = 2;
}

message Person {
    string name = 1;
    int32 id = 2;
    string email = 3;
    repeated PhoneNumber phones = 4;
}

message AddressBook {
    repeated Person people = 1;
}
```

### How to run
1. Build, Install & Load module
```shell
cd address_book/module
make
make install
make load
```

2. Start server
```shell
cd address_book/user_space
make server
```

3. Start client
```shell
cd address_book/user_space
make client
```