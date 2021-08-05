package main

import (
	"fmt"
	"sync"
	"runtime"
	)

type UserAges struct {
	ages map[string]int
	// sync.RWMutxex will be better
	sync.Mutex
}

func (ua *UserAges) Add(name string, age int) {
	ua.Lock()
	defer ua.Unlock()
	ua.ages[name] = age
}

func (ua *UserAges) Get(name string) int {
	ua.Lock()
	defer ua.Unlock()
	if age, ok := ua.ages[name]; ok {
		return age
	}
	return -1
}
