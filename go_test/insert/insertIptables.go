package main

import (
	"os/exec"
    "time"
	"fmt"
	"flag"
)


func insert( T chan int64, port int){
    var err error
    var cmd *exec.Cmd
	s := time.Now()
	//cmd = exec.Command("/user/sbin/iptables", "-A", "IPTABLES_TEST_INPUT", "-p", "tcp", "-s", "10.92.86.172", "--dport", string(port), "-j", "DROP")
	cmd = exec.Command("/bin/bash", "./insert.sh")
    if _, err = cmd.Output(); err != nil {
        fmt.Println(err)
		T <- 0
		return
	}
	T <- time.Since(s).Milliseconds()
}

func runInsert(num int){
	T := make(chan int64)
	for i := 0; i < num; i++ {
        go insert(T, i + 50000)
	}
	var sum int64 = 0
	var tmp int64 = 0
	var maxTime int64 = 0
	var minTime int64 = 60000
	for i := 0; i < num; i++ {
        tmp = (<-T)
		sum += tmp
		if maxTime < tmp {
			maxTime = tmp
		}
		if minTime > tmp {
			minTime = tmp
		}
	}
	fmt.Printf("Insert %d iptables speed %d ms, max one: %d, min one:%d\n", num, sum, maxTime, minTime)
}

func main() {
	var insertNum *int
	
	insertNum = flag.Int("n", 1, "insert number")
	flag.Parse()
	fmt.Println("Start ...")
	runInsert(*insertNum)
}
