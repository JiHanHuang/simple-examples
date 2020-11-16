package main

import (
	"fmt"
	"sync"
)

var step = 5
var threads = 3
var start = 1
var finish = 90
var wg sync.WaitGroup

func outNum(num int, s, e chan int){
	for {
		a :=<- s
		if (a >= finish){
			e <- a
			fmt.Println("threads ", num, " finish")
			break
		}
		for i:=0;i<step;i++{
			fmt.Println("threads ", num, ": ", a+i)
		}
		e <- a + step
	}
	wg.Done()
}

func main(){
	if finish / 15 != 0{
		fmt.Println("finish num is invild")
	}
	// var key [10](chan int)
	// for i:=0;i<10;i++{
	// 	key[i] = make(chan int)
	// }
	key1 := make(chan int,1)
	key2 := make(chan int)
	key3 := make(chan int)
	//defer close(key1)
	wg.Add(threads)
	key1 <- 1
	go outNum(1,key1, key2)
	go outNum(2,key2, key3)
	go outNum(3,key3, key1)
	wg.Wait()
}