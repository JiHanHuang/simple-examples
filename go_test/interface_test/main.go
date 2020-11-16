package main

import(
	"fmt"
	"go_test/interface_test/itHttp"
	"go_test/interface_test/m"
)

const(
	ModuleTest int = iota
)

func init()  {
	InitHttpListenConfig(ModuleTest, "0.0.0.0", "8000")
	HttpAddHandler(ModuleTest, m.NewHTTPTest())
}

func httpServer(h *itHttp.HTTPListen){
	fmt.Println("HTTP ",h.HTTPHost, " Listening...")
	fmt.Println(h.ListenAndServe())
}

func main() {
	for _, v := range conf.HttpListen {
		go httpServer(v)
	}
	ch := make(chan struct{})
	<- ch
}