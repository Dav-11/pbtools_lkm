package main

import (
	"fmt"
	"net"
	"bytes"

	"address_book/gen"

	"google.golang.org/protobuf/proto"
)

const (
	port int = 60001
)

func main() {

	// create protobuf
	person := gen.Person{
		Name: "Kalle Kula",
		Id: 56,
		Email: "kalle.kula@foobar.com",
		Phones: []*gen.Person_PhoneNumber{
			{
				Number: "+46701232345",
				Type: gen.Person_HOME,
			},
			{
				Number: "+46999999999",
				Type: gen.Person_WORK,
			},
		},
	}

	book := gen.AddressBook{
		People: []*gen.Person{
			&person,
		},
	}

	// Marshal the AddressBook into bytes
    data, err := proto.Marshal(&book)
    if err != nil {
        panic(err)
    }

	// Connect to UDP server
	address := fmt.Sprintf("127.0.0.1:%d", port)
    conn, err := net.Dial("udp", address)
    if err != nil {
        panic(err)
    }
    defer conn.Close()

	var buffer bytes.Buffer

	buffer.Write(data)

	// Send the encoded data
    if _, err = conn.Write(buffer.Bytes()); err != nil {

		panic(err)
	}
}