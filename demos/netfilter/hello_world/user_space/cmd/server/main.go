package main

import (
	"fmt"
	"net"
)

const (
	port int = 60001
)

func main() {

	address := fmt.Sprintf(":%d", port)

	// Listen on TCP port 60001
	ln, err := net.Listen("tcp4", address)
	if err != nil {
		
		fmt.Println("Error listening:", err)
		return
	}
	defer ln.Close()

	fmt.Printf("listening on [%s]...\n", address)

	for {
		// Accept a connection
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println("Error accepting connection:", err)
			continue
		}
		defer conn.Close()

		// Receive data from client
		data := make([]byte, 1024) // Buffer for receiving data
		n, err := conn.Read(data)
		if err != nil {
			fmt.Println(err)
			continue
		}

		// Convert received data to string (remove trailing null bytes)
		message := string(data[:n])
		fmt.Printf("Received from client: %s\n", message)
	}
}