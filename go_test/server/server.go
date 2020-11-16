package main

import (
    "net"
    "time"
	"fmt"
	"sync"
	"flag"

	//linuxproc "github.com/c9s/goprocinfo/linux"
)

var rwL *sync.RWMutex
var errNum int = 0
var connNum int = 0
var debug bool

func handleConnection(conn net.Conn){
	time.Sleep(600 * time.Second)
	conn.Close()
}

func runServer(host string, port string, lc bool){
	ln, err := net.Listen("tcp", host + ":" + port)
	if err != nil{
		fmt.Println(err.Error())
		return
	}
	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println(err.Error())
			rwL.Lock()
			errNum ++
			rwL.Unlock()
		}
		rwL.Lock()
		connNum ++
		rwL.Unlock()
		if debug {
			fmt.Println("Client IP:", conn.RemoteAddr())
		}
		if lc{
			go handleConnection(conn)
		}else{
			conn.Close()
		}
	}
}

func outPut(loopTime int){
	for{
		time.Sleep(time.Second * time.Duration(loopTime))
		fmt.Println(time.Now().Format("2006-01-02 15:04:05"))
		rwL.RLock()
		fmt.Printf("Total connections: %d\n", connNum)	
		fmt.Printf("Error times: %d \n", errNum)
		rwL.RUnlock()
		rwL.Lock()
		errNum = 0
		connNum = 0
		rwL.Unlock()

/*		stat, err := linuxproc.ReadStat("/proc/stat")
		if err != nil {
			fmt.Println("stat read fail")
			return
		}

		for _, s := range stat.CPUStats {
			fmt.Println("User", s.User, "Nice", s.Nice, "System", s.System, "Idle", s.Idle, "IOWait", s.IOWait)
		}*/
		fmt.Println("-------------------------------------")
	}
}

func main() {
	var host string
	var port string
	var loopTime int
	var longConn bool
	
	flag.StringVar(&port, "dport", "8888", "listen port")
	flag.StringVar(&host, "host", "0.0.0.0", "server host")
	flag.IntVar(&loopTime, "lt", 2, "loop time (s)")
	flag.BoolVar(&longConn, "lc", false, "long connection")
	flag.BoolVar(&debug, "debug", false, "debug mode")

	flag.Parse()
	//flag.Usage()

	rwL = new(sync.RWMutex)

	fmt.Printf("%s:%s Listening...\n",host, port)
	go outPut(loopTime)
	runServer(host, port, longConn)
}
