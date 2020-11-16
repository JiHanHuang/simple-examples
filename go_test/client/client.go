package main

import (
    "net"
    "time"
	"fmt"
	"sync"
	"flag"
	"math/rand"

	//linuxproc "github.com/c9s/goprocinfo/linux"
)

var rwL *sync.RWMutex
var errNum int = 0
var localIP = [...]string{"192.168.17.240", "192.168.17.241", "192.168.17.242", "192.168.17.243", "192.168.17.244"}

func dialCustom(network, address string) (net.Conn,error) {
//	netAddr := new(net.TCPAddr)

	i := rand.Intn(len(localIP))
	//fmt.Println(localIP[i])
//	netAddr.IP = []byte(localIP[i])

//	fmt.Println("netAddr:", netAddr, netAddr.IP)

	netAddr, _ := net.ResolveTCPAddr("tcp", localIP[i])

	d := net.Dialer{LocalAddr: netAddr}
	return d.Dial("tcp", address)
}

func connectOne(host string, port string, T chan float64, lc bool) {
	s := time.Now()
//	conn, err := net.Dial("tcp", host + ":" + port)
	conn, err := dialCustom("tcp", host + ":" + port)
	if err != nil{
		fmt.Println(err.Error())
		rwL.Lock()
		errNum ++
		rwL.Unlock()
		T <- 0
		return
	}
	if lc {
		for i:=0; i<600; i++ {
			time.Sleep(time.Second)
			conn.Write([]byte(fmt.Sprintf("%d", i)))
		}
	}
	conn.Close()
	T <- time.Since(s).Seconds()
}

func connectKeep(host string, port string, step int, T chan float64) {
	for {
		s := time.Now()
		conn, err := net.Dial("tcp", host + ":" + port)
		if err != nil{
			fmt.Println(err.Error())
			rwL.Lock()
			errNum ++
			rwL.Unlock()
			T <- 0
			return
		}else {
			conn.Close()
			T <- time.Since(s).Seconds()
		}
		sleep := time.Duration(1000 / step)
		time.Sleep(sleep * time.Millisecond)
	}
}

func oneStep(host string, port string, threads int, lc bool){
	T := make(chan float64)
    for i := 0; i < threads; i++ {
        go connectOne(host, port, T, lc)
	}
	var sum float64 = 0
	var tmp float64 = 0
	var maxTime float64 = 0
	var minTime float64 = 60
	for i := 0; i < threads; i++ {
		//fmt.Println(i, " >",sum)
		tmp = (<-T)
		sum += tmp
		if maxTime < tmp {
			maxTime = tmp
		}
		if minTime > tmp {
			minTime = tmp
		}
	}
	outPut(maxTime, minTime, sum, threads)
}

func keepPress(host string, port string, threads int, counts int, loopTime int){
	connNum := 0
	T := make(chan float64)
	if counts < threads {
		counts = threads
		threads = counts / 2
	}
    for i := 0; i < threads; i++ {
        go connectKeep(host, port, counts/threads, T)
	}
	var sum float64 = 0
	var tmp float64 = 0
	var maxTime float64 = 0
	var minTime float64 = 60
	s := time.Now()
	for {
		tmp = (<-T)
		if tmp < 0.0000001 {
			return
		}
		sum += tmp
		if maxTime < tmp {
			maxTime = tmp
		}
		if minTime > tmp {
			minTime = tmp
		}
		connNum ++
		if time.Since(s).Milliseconds() > int64(loopTime * 1000) {
			outPut(maxTime, minTime, sum, connNum)
			s = time.Now()
			sum, connNum =  0, 0
			minTime, maxTime = 60, 0
		}
	}
}

func outPut(max float64, min float64, total float64, connNum int){
	fmt.Printf("Total connections: %d\n", connNum)	
	rwL.Lock()
	fmt.Printf("Error times: %d \n", errNum)
	errNum = 0
	rwL.Unlock()
	fmt.Printf("Total time: %0.2f ms\n", total * 1000)
	fmt.Printf("Max time: %0.2f, Min time: %0.2f, Average time: %0.2f ms\n", max * 1000, min * 1000, total * 1000 / float64(connNum))

/*	stat, err := linuxproc.ReadStat("/proc/stat")
	if err != nil {
		fmt.Println("stat read fail")
		return
	}

	for _, s := range stat.CPUStats {
		fmt.Println("User", s.User, "Nice", s.Nice, "System", s.System, "Idle", s.Idle, "IOWait", s.IOWait)
	}*/
	fmt.Println("-------------------------------------")
}

func main() {
	var host string
	var port string
	var runType string
	var threads int
	var counts int
	var loopTime int
	var oneLoop int
	var longConn bool
	
	flag.StringVar(&port, "dport", "60001", "dst port")
	flag.StringVar(&host, "host", "10.92.2.239", "dst host")
	flag.StringVar(&runType, "type", "one", "run type one or keep")
	flag.IntVar(&threads, "t", 2, "threads")
	flag.IntVar(&counts, "c", 2, "counts")
	flag.IntVar(&loopTime, "lt", 1, "loop time (s)")
	flag.IntVar(&oneLoop, "ol", 1, "one loop num")
	flag.BoolVar(&longConn, "lc", false, "long connection")
	flag.Parse()
	//flag.Usage()

	rwL = new(sync.RWMutex)
	rand.Seed(time.Now().Unix())

	fmt.Printf("Start ... dst:%s:%s, type:%s, threads:%d\n",host, port, runType, threads)
	if runType == "keep"{
		keepPress(host, port, threads, counts, loopTime)
	}else{
		for i := 0; i < oneLoop; i++ {
			oneStep(host, port, threads, longConn)
			time.Sleep(time.Duration(loopTime) * time.Second)
		}
	}
}
