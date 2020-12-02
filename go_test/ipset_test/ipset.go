package main

// #cgo LDFLAGS: -L${SRCDIR}/ -Wl,-rpath,${SRCDIR}/ -lcipset -L/usr/lib64/  -lipset
// #include <stdlib.h>
// #include "cipset.h"
import (
	"C"
)
import "unsafe"

func main() {
	cip := C.CString("10.92.2.100")
	defer C.free(unsafe.Pointer(cip))
	var caddr C.struct_address
	C.ip2addr(cip, &caddr)
	//caddr.family = C.AF_INET
	//C.inet_aton(cip, caddr.ip4_addr)
	var cstate *C.struct_ipset_state = C.ipset_init()
	C.ipset_add_ip(cstate, &caddr)
	C.ipset_fini(cstate)
}
