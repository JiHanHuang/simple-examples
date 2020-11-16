package m

import(
	"go_test/interface_test/itHttp"
	"net/http"
	"fmt"
)

type HTTPTest struct {
	test string
}

func NewHTTPTest() (* HTTPTest) {
	return &HTTPTest{}
}

func (t *HTTPTest) HTTPHandler(h *itHttp.HTTPListen)  {
	h.Serve.Handle("/hello", http.HandlerFunc(t.HandlerEcho))
}

func (t* HTTPTest) HandlerEcho(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "helloxxxxx!\n")
    //w.Write([]byte("hello!"))
}