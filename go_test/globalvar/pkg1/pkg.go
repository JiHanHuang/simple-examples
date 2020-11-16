package pkg1

import (
	"fmt"
	"go_test/globalvar/global"
)

func Print()  {
	global.Global = 111
	fmt.Println("pkg1:", global.Global)
}