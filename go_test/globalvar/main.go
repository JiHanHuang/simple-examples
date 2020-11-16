package main

import (
	"fmt"
	"go_test/globalvar/global"
	"go_test/globalvar/pkg1"
)


func main()  {
	pkg1.Print()
	fmt.Println("main: ", global.Global)
}