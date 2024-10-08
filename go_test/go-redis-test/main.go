package main

import (
	"context"
	"flag"
	"fmt"
	"strings"
	"sync"
	"time"

	"github.com/go-redis/redis/v8"
)

var ctx = context.Background()

func main() {
	redisAddrs := flag.String("h", "localhost:6379", "Comma-separated Redis addresses")
	numberOfOperations := flag.Int("n", 1000000, "Number of operations to perform")
	numberOfGoroutines := flag.Int("threads", 100, "Number of goroutines for concurrent execution")
	valueSize := flag.Int("d", 2048, "Size of the value in bytes to store with SET operations")
	flag.Parse()

	operations := []string{"SET", "GET", "ZADD"}
	for _, operation := range operations {
		fmt.Printf("Testing %s\n", operation)
		performOperationTest(*redisAddrs, operation, *numberOfOperations, *numberOfGoroutines, *valueSize)
	}
}

func testRedisClient(addrs []string) redis.UniversalClient {
	if len(addrs) > 1 {
		return redis.NewClusterClient(&redis.ClusterOptions{
			Addrs: addrs,
		})
	}
	return redis.NewClient(&redis.Options{
		Addr: addrs[0],
		DB:   0,
	})
}

func performOperationTest(redisAddrs string, operation string, numberOfOperations, numberOfGoroutines, valueSize int) {
	var wg sync.WaitGroup
	opsPerGoroutine := numberOfOperations / numberOfGoroutines

	startTime := time.Now()
	value := strings.Repeat("a", valueSize)

	for g := 0; g < numberOfGoroutines; g++ {
		wg.Add(1)
		go func(goroutineID int) {
			defer wg.Done()

			addrs := strings.Split(redisAddrs, ",")
			client := testRedisClient(addrs)
			defer client.Close()

			for i := 0; i < opsPerGoroutine; i++ {
				key := fmt.Sprintf("key:{%d}:%s:%d", goroutineID, operation, i)
				switch operation {
				case "SET":
					client.Set(ctx, key, value, 0).Result()
				case "GET":
					key = strings.ReplaceAll(key, "GET", "SET")
					client.Get(ctx, key).Result()
				case "ZADD":
					client.ZAdd(ctx, key, &redis.Z{Score: float64(i), Member: value}).Result()
				}
			}
		}(g)
	}

	wg.Wait()
	duration := time.Since(startTime)
	opsPerSecond := float64(numberOfOperations) / duration.Seconds()
	fmt.Printf("%s: %d operations with %d goroutines took %s. Throughput: %.2f ops/sec\n", operation, numberOfOperations, numberOfGoroutines, duration, opsPerSecond)
}

// go build -o redis-test-go
