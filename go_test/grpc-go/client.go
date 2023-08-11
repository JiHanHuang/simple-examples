package main

import (
	"bufio"
	"context"
	"fmt"
	"os"

	"go_test/grpc-go/proto"

	"google.golang.org/grpc"
)

func main() {

	var serviceHost = "127.0.0.1:8001"

	conn, err := grpc.Dial(serviceHost, grpc.WithInsecure())
	if err != nil {
		fmt.Println(err)
	}
	defer conn.Close()

	client := proto.NewHelloClient(conn)
	rsp, err := client.Say(context.TODO(), &proto.SayRequest{
		Name: "BOSIMA",
	})

	if err != nil {
		fmt.Println(err)
	}

	fmt.Println(rsp)

	fmt.Println("按回车键退出程序...")
	in := bufio.NewReader(os.Stdin)
	_, _, _ = in.ReadLine()
}
