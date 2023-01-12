package goroutine

import "fmt"

func Run() {
	go run1()
	go run2()
}

func run1() {
	for i := 0; i < 10; i++ {
		fmt.Println("run 1:", i)
	}
}

func run2() {
	for i := 0; i < 10; i++ {
		fmt.Println("run 2:", i)
	}
}
