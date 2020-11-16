package itHttp

import (
	"net/http"
	//"errors"
)

type HTTPHandler interface{
	HTTPHandler(h *HTTPListen)
}

//handle???
type HTTPListen struct {
	HTTPHost string
	HTTPPort string
	Serve *http.ServeMux
}

func NewHTTPListen() (*HTTPListen) {
	return &HTTPListen{HTTPHost:"127.0.0.1", 
		HTTPPort:"8000", Serve:http.NewServeMux()}
}

func (h *HTTPListen) AddHandler(handler HTTPHandler)  {
	handler.HTTPHandler(h)
}

func (h *HTTPListen) ListenAndServe() (error) {
	//todo ipv4, ipv6
	return http.ListenAndServe(h.HTTPHost + ":" + h.HTTPPort, h.Serve)
}