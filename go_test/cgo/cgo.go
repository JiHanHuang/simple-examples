package main

// #include <stdlib.h>
import (
	"C"
)
import "fmt"

func main() {
	fmt.Println(int(C.random()))
}
