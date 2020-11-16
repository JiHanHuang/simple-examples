package main

import (
	"fmt"
	"time"
)

func main() {
	tstr := "2020-06-11T09:50:24+08:00"
	t2 := time.Now()
	t1, _ := time.Parse(time.RFC3339, tstr)
	fmt.Println(t1, t2)
	fmt.Println(t1.After(t2), t2.Before(t1))
}
