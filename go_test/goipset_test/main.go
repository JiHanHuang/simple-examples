/*
 * @Author: JiHan
 * @Date: 2021-03-08 09:57:25
 * @LastEditTime: 2021-03-08 10:35:42
 * @LastEditors: JiHan
 * @Description:
 * @Usage:
 */

package main

import (
	"log"
	"net"

	"github.com/JiHanHuang/goipset"
)

func main() {
	ipsetName := "test"
	protocol, err := goipset.Protocol()
	check(err)
	log.Println("Protocol:", protocol)

	err = goipset.Create(ipsetName, "hash:ip", goipset.GoIpsetCreateOptions{})
	check(err)

	entry := goipset.GoIPSetEntry{
		Set: &goipset.SetIP{IP: net.ParseIP("1.1.1.1")},
	}
	err = goipset.Add(ipsetName, &entry)
	check(err)

	result, err := goipset.List(ipsetName)
	check(err)
	log.Printf("List:%v", result.Entries)
}

func check(err error) {
	if err != nil {
		log.Fatalf("Err: %v", err)
	}
}
