# Netfilter

## Instructions

1. CD into example directory
```shell
cd <example_folder>
```
2. Setup python venv and install requirements
```shell
make -C module requirements
```
3. Regenerate code from protos
```shell
make -C module generate
```
4. Compile module
```shell
make -C module
```
5. Install module
```shell
make -C module install
```
6. Load module
```shell
make -C module load
```
7. Start server app
```shell
make -c user_space server 
```
8. Start the client (in a new shell)
```shell
make -c user_space client
```

## Examples

### Hello World
This hook drops all packages where bar > 50

#### Proto
```proto
message Foo {
    int32 bar = 1;
}
```


### Address Book
Drops packages that have less than 2 `WORK` phone number inside the the first `Person.Phones`.

#### Proto
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
