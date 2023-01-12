package main

import (
	"fmt"
	"net/http"
	_ "net/http/pprof"
	"os"

	"gohello.jihan/goroutine"
	"gohello.jihan/hello"
	"gohello.jihan/httpserver"
	"gohello.jihan/routinewait"
)

func main() {
	var m map[int]bool
	m[0]=false
	if len(os.Args) < 2 {
		fmt.Println("Please input arg [hello/http/go/gowait]")
		return
	}
	if len(os.Args) > 2 {
		go func() {
			fmt.Println(http.ListenAndServe(":8091", nil))
		}()
		fmt.Println("pprof start...")
	}
	name := os.Args[1]
	switch name {
	case "hello":
		hello.Hello()
		h := hello.NewHello("jihan")
		h.Hello()
	case "http":
		httpserver.Run()
	case "go":
		goroutine.Run()
	case "gowait":
		routinewait.Run()
	default:
		fmt.Println("Invild input", name)
	}
	fmt.Println("Run over")
}
