package main

import (
	"fmt"
	"runtime"
	"time"
)

func main() {
	runtime.GOMAXPROCS(1)
	go func() {
		fmt.Println("proc 1 start")
		time.Sleep(10 * time.Second)
		fmt.Println("proc 1 end")
	}()
	time.Sleep(1 * time.Second)
	go func() {
		fmt.Println("proc 2 start")
		fmt.Println("proc 2 end")
	}()
	time.Sleep(11 * time.Second)
	fmt.Println("END")
}
