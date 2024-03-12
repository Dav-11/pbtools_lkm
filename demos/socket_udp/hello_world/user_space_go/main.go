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

	for true {

		fmt.Println("input number:")
    	var input int32
    	_, err := fmt.Scanf("%d", &input)
		if err != nil {
			panic(err)
		}

    	fmt.Printf("read int: %d\n", input)

		// create protobuf
		foo := gen.Foo{
			Bar: input,
		}

		// Marshal the Foo into bytes
		data, err := proto.Marshal(&foo)
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
}