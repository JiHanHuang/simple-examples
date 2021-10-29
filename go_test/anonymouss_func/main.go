package main

import "fmt"

func squares() func() int {
	var x int
	return func() int {
		x++
		return x * x
	}
}

func main() {
	f := squares()
	fmt.Println(f()) //1
	fmt.Println(f()) //4
	fmt.Println(f()) //9

	f2 := squares()
	fmt.Println(f2()) //1
	fmt.Println(f2()) //4
	fmt.Println(f2()) //9
}
