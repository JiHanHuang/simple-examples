package routinewait

import (
	"fmt"
	"sync"
)

var j int
var r1 chan struct{}
var r2 chan struct{}

func Run() {
	r1 = make(chan struct{})
	r2 = make(chan struct{})
	wg := sync.WaitGroup{}
	wg.Add(2)
	go func() {
		run1()
		r1 <- struct{}{}
		wg.Done()
	}()
	go func() {
		run2()
		wg.Done()
	}()
	//<-r1
	wg.Wait()
}

func run1() {
	j++
	for i := 0; i < 10; i++ {
		r1 <- struct{}{}
		fmt.Println("run 1:", i)
		<-r2
	}
}

func run2() {
	j++
	for i := 0; i < 10; i++ {
		r2 <- struct{}{}
		fmt.Println("run 2:", i)
		<-r1
	}
}
