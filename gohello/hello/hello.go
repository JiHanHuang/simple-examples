package hello

import "fmt"

func Hello() {
	fmt.Println("hello you")
}

type H struct {
	name string
}

func NewHello(name string) *H {
	return &H{name: name}
}

func (h *H) Hello() {
	fmt.Println("hello", h.name)
}
