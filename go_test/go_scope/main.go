/*
 * @Author: JiHan
 * @Date: 2021-08-05 09:54:58
 * @LastEditTime: 2021-08-05 14:24:28
 * @LastEditors: JiHan
 * @Description:
 * @Usage:
 */

package main

import (
	"fmt"
	"go_test/go_scope/func_scope"
	"go_test/go_scope/var_scope"
)

func main() {
	printVar()
	runFunc()
}

type SendVar struct {
	Out   int
	inner int
}

var fun func(int) error

func printVar() {
	s := SendVar{
		1,
		2,
	}
	var_scope.PrintOtherStruct(s)
	vs := var_scope.VS{Out: 1}
	fmt.Println("Print var_scope VS", vs)
	//func_scope.CallBack(fun)//出错，因为赋值fun的s.print中的s已经被释放
}

func runFunc() {
	s := SendVar{
		1,
		2,
	}
	func_scope.CallBack(func(i int) error {
		fmt.Println("inner call back func", s.inner, s.Out)
		return nil
	})
	func_scope.CallBack(print) //可以使用
	fun = s.print
	func_scope.CallBack(fun) //可以使用
}

func (s SendVar) print(i int) error {
	fmt.Println("call back print func", s.inner, s.Out)
	return nil
}

func print(i int) error {
	fmt.Println("call back print func", i)
	return nil
}
