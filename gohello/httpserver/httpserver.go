package httpserver

import (
	"fmt"
	"net/http"
)

func Hello(w http.ResponseWriter, req *http.Request) {

	fmt.Fprintf(w, "hello\n")
}

func Headers(w http.ResponseWriter, req *http.Request) {

	for name, headers := range req.Header {
		for _, h := range headers {
			fmt.Fprintf(w, "%v: %v\n", name, h)
		}
	}
}

func Run() {

	http.HandleFunc("/hello", Hello)
	http.HandleFunc("/headers", Headers)

	http.ListenAndServe(":8090", nil)
}
