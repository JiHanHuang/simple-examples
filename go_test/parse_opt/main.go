package main

import (
	"go_test/parse_opt/opt"
	"strconv"
)

func main() {
	s := opt.NewStructerOpt(1, opt.Opt{A: 1, B: "1"})
	s.PrintOpt()
	y := opt.NewStructerFun(2, opt.SetAOpt(2), opt.SetBOpt("2"))
	y.PrintOpt()
	k := opt.NewStructerFun(3, setOpt(3))
	k.PrintOpt()
}

func setOpt(n int) opt.OptFun {
	return func(o *opt.Opt) {
		o.A = n
		o.B = strconv.Itoa(n)
	}
}
