package opt

import "fmt"

type Opt struct {
	A int
	B string
}

type Structer struct {
	a   int
	opt Opt
}

type OptFun func(o *Opt)

//这种方式，配置仅仅是固定值的时候，非常合适
func NewStructerOpt(num int, opt Opt) *Structer {
	return &Structer{a: num, opt: opt}
}

//这种方式，配置需要一些自定义操作，或者涉及多个配置的合作处理
func NewStructerFun(num int, ofs ...OptFun) *Structer {
	opt := Opt{}
	for _, of := range ofs {
		of(&opt)
	}
	return &Structer{a: num, opt: opt}
}

func (s *Structer) PrintOpt() {
	fmt.Println("OPT: ", s.opt)
}

func SetAOpt(a int) OptFun {
	return func(o *Opt) {
		o.A = a
	}
}

func SetBOpt(b string) OptFun {
	return func(o *Opt) {
		o.B = b
	}
}
