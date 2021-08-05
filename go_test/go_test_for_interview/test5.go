package main

import (
	"fmt"
	"runtime"
	"time"
	)

func main() {
	runtime.GOMAXPROCS(1)
	int_chan := make(chan int, 1)
	string_chan := make(chan string, 1)
	int_chan <- 1
	string_chan <- "hello"
	select {
	case value := <-int_chan:
		fmt.Println(value)
	case value := <-string_chan:
		panic(value)
	case value := <-sleep():
		fmt.Println("sleep:", value)
	}
}

func sleep() chan int{
	ch := make(chan int, 5)
	time.Sleep(5* time.Second)
	return ch
}
