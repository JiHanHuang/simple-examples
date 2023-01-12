/*
 * @Author: JiHan
 * @Date: 2021-11-19 10:32:23
 * @LastEditTime: 2021-11-19 10:34:40
 * @LastEditors: JiHan
 * @Description:
 * @Usage:
 */

package main

import (
	"fmt"
	"time"
)

var open chan bool

func send() {
	time.Sleep(time.Second)
	open <- true
	fmt.Println("send open", time.Now())
}

func main() {
	fmt.Println(time.Now())
	tm := time.NewTimer(10 * time.Second)
	open = make(chan bool)
	go send()
	for {
		select {
		case <-tm.C:
			fmt.Println("time up", time.Now())
			tm = time.NewTimer(10 * time.Second)
		case <-open:
			//creat 1s timer and tm.C will be time up after 1s
			tm = time.NewTimer(time.Second)
		}
	}
}
