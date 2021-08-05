package main

import "fmt"


type student struct {
	Name string
	Age  int
}

func main() {
	m := make(map[string]*student)
	stus := []student{
		{Name: "zhou", Age: 24},
		{Name: "li", Age: 23},
		{Name: "wang", Age: 22},
	}
	for i, stu := range stus {
		m[stu.Name] = &stu
	}
	fmt.Println(m)
	fmt.Println(*m["li"])
}
