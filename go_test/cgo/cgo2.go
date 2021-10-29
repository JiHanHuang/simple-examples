package main

//该示例参考来源：Go语言程序设计

/*
#cgo CFLAGS: -I/usr/include
#cgo LDFLAGS: -L/usr/lib -lbz2
#include <bzlib.h>

int bz2compress(bz_stream *s, int action,
	char *in, unsigned *inlen, char *out, unsigned *outlen);
*/

//标识加入cgo，程序会分析注释中的相关内容
import "C"
import (
	"io"
	"unsafe"
)

type writer struct {
	w      io.Writer
	stream *C.bz_stream
	outbuf [64 * 1024]byte
}

//NewWriter 对于bzip2压缩的流返回一个wirter
func NewWriter(out io.Writer) io.WriteCloser {
	const (
		blockSize  = 9
		verbosity  = 0
		workFactor = 30
	)
	w := &writer{w: out, stream: C.bz2alloc()}
	C.BZ2_bzCompressInit(w.stream, blockSize, verbosity, workFactor)
	return w
}

func (w *writer) Write(data []byte) (int, error) {
	if w.stream == nil {
		panic("closed")
	}
	var total int
	for len(data) > 0 {
		inlen, outlen := C.uint(len(data)), C.uint(cap(w.outbuf))
		C.bz2compress(w.stream, C.BZ_RUN,
			(*C.char)(unsafe.Pointer)(&data[0]), &inlen,
			(*C.char)(unsafe.Pointer)(&w.outbuf), &outlen)
		total += int(inlen)
		data = data[inlen:]
		if _, err := w.w.Write(w.outbuf[:outlen]); err != nil {
			return total, err
		}
	}
	return total, nil
}
