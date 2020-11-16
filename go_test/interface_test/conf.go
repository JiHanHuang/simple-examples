package main

import(
	"go_test/interface_test/itHttp"
)

type Config struct {
	HttpListen map[int]*itHttp.HTTPListen
}

var conf Config

func init()  {
	conf.HttpListen = make(map[int]*itHttp.HTTPListen)
}

func InitHttpListenConfig(id int, host, port string) (error) {
	conf.HttpListen[id] = itHttp.NewHTTPListen()
	conf.HttpListen[id].HTTPHost = host
	conf.HttpListen[id].HTTPPort = port
	return nil
}

func HttpAddHandler(id int, h itHttp.HTTPHandler)  {
	conf.HttpListen[id].AddHandler(h)	
}