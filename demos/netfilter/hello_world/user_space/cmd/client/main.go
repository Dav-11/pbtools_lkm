package main

import (
	"fmt"
	"net"
	"time"
)

const (
	port int = 60001
)

func main() {

	for {

		// Connect to TCP server
		address := fmt.Sprintf(":%d", port)
		conn, err := net.Dial("tcp4", address)
		if err != nil {
			panic(err)
		}
    	defer conn.Close()

		// Message to send
		message := "Hello from client!"
	
		// Send message to server
		_, err = conn.Write([]byte(message))
		if err != nil {
			fmt.Println(err)
			return
		}

		fmt.Printf("Sent message to %s\n", address)

		// Wait for 5 seconds before next send/receive
		time.Sleep(5 * time.Second)
	}
}