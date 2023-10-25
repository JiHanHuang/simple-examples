package main

import "fmt"

func main() {
	fmt.Println("run test:", test()) //2
}

func add(in *int) int {
	return *in + 1
}

func test() int {
	var a = 0
	fmt.Println("start", a) //0
	a += 1
	defer func() {
		fmt.Println("defer", a) // 1
	}()
	return add(&a)
}
