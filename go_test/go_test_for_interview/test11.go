package main

import (
	"fmt"
	)

type People interface {
	Show()
}

type Student struct{}

func (stu *Student) Show() {

}

func live() People {
	var stu *Student
	return stu
}

func main() {
	//check interface is nil or not. 
	if live() == nil {
		fmt.Println("AAAAAAA")
	} else {
		fmt.Println(live())//nil
		fmt.Println("BBBBBBB")
	}
}

//better way to check interface is nil or not
//https://blog.csdn.net/lanyang123456/article/details/83715090
func IsNil(i interface{}) bool {
    vi := reflect.ValueOf(i)
    if vi.Kind() == reflect.Ptr {
        return vi.IsNil()
    }
    return false
}
