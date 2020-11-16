package main

import (
	"fmt"
	"time"
	"sync"
	"context"
)
var sm sync.Mutex
var kk int = 0
func work(flag string, ctx context.Context){
	i:=1
	for{
		time.Sleep(1 * time.Second)
		select{
		case <- ctx.Done():
			fmt.Println(flag, "finish")
			return
		default:
			fmt.Println(flag, " work:",i,kk)
			i+=1
			sm.Lock()
			kk +=1
			sm.Unlock()
		}
	}
}

func timeout(ticker *time.Ticker,flag string, ctx context.Context){
	i:=1
	for{
		// if timeout, ok := ctx.Deadline();ok{
		// 	fmt.Println("time left:", timeout.Sub(time.Now()))
		// }
		select{
		case <- ctx.Done():
			flag += "kkkkkkkk"
			fmt.Println(flag, "finish")
			return
		case <- ticker.C:
			fmt.Println(flag, " work:",i,kk)
			i+=1
			sm.Lock()
			kk +=1
			sm.Unlock()
		}
	}
}

func main(){
	fmt.Println("start")
	ctxC, cancelC := context.WithCancel(context.Background())
	ctxD, cancelD := context.WithDeadline(context.Background(), time.Now().Add(5*time.Second))
	ctxT, cancelT := context.WithTimeout(context.Background(), 3*time.Second)
	go work("concel",ctxC)
	go work("dead",ctxD)
	kk := "timeout"
	ticker := time.NewTicker(time.Second)
	go timeout(ticker,kk,ctxT)
	time.Sleep(10*time.Second)
	cancelC()
	cancelD()
	cancelT()
	time.Sleep(2*time.Second)
	fmt.Println("end",kk)
}