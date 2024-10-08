package main

import (
	"context"
	"fmt"
	"log"
	"strings"
	"time"

	"github.com/go-redis/redis/v8"
)

const RDB_CFG_DB = 0
const localIP = "192.168.192.131:6379"
const localPWD = "Gsta@1234"
const dupfIP1 = "172.29.3.102:6379"
const dupfIP2 = "172.29.3.103:6379"

// const dupfPWD = "Gsta@1234"
const dupfPWD = "zdx5GC18"

func main() {
	// 创建连接池
	pool := redis.NewClient(&redis.Options{
		Addr:     dupfIP1,
		Password: dupfPWD,
		DB:       RDB_CFG_DB,
	})

	shutdown, cancel := context.WithCancel(context.Background())

	// 订阅一个频道
	go subscribe(shutdown, pool, "MSG_TRACK_TASK|*")
	// subscribe(shutdown, pool, "MSG_TRACK_TASK|*")
	// subscribe2(shutdown, pool, "MSG_TRACK_TASK|*")
	fmt.Println("sub success")
	time.Sleep(time.Second)
	publish(shutdown, pool, "MSG_TRACK_TASK|", "jihan-test")
	fmt.Println("pub data")
	time.Sleep(time.Second * 1)
	cancel()
	time.Sleep(time.Second)
	fmt.Println("Over")
}

// 订阅一个频道
func subscribe(ctx context.Context, pool *redis.Client, channel string) {

	psc := pool.PSubscribe(ctx, fmt.Sprintf("__keyspace@%d__:%s", RDB_CFG_DB, channel))
	defer psc.Close()

	for {
		v, err := psc.Receive(ctx)
		if err != nil {
			fmt.Println("receive error:", err)
			return
		}
		switch data := v.(type) {
		case *redis.Subscription:
			// subscribe succeeded
			fmt.Println("Subscription success. channel:", data.Channel)
		case *redis.Message:
			fmt.Printf("Received message on %s: %s.\n", data.Channel, data.Payload)
			// 执行一些任务
			keys := strings.Split(data.Channel, ":")
			if len(keys) < 2 {
				fmt.Println("receive unknown channel:", err)
				return
			}
			switch strings.ToLower(data.Payload) {
			case "hset":
				// err := pool.HMGet(ctx, keys[1], )
				fileds, err := pool.HGetAll(ctx, keys[1]).Result()
				if err != nil {
					fmt.Println("hmget failed:", err)
					return
				}
				fmt.Println("hset data:", fileds)
			default:
				fmt.Println("unknown payload type:", data.Payload)
			}

		default:
			fmt.Println("unknown receive type:", v)
			return
		}
	}
}

// 订阅一个频道
func subscribe2(ctx context.Context, pool *redis.Client, channel string) {

	psc := pool.PSubscribe(ctx, fmt.Sprintf("__keyspace@%d__:%s", RDB_CFG_DB, channel))
	ch := psc.Channel()

	go func() {

		defer psc.Close()
		var data *redis.Message
		for {
			select {
			case <-ctx.Done():
				fmt.Println("shutdown")
				return
			case data = <-ch:
				fmt.Printf("Received message on %s: %s.\n", data.Channel, data.Payload)
				// 执行一些任务
				keys := strings.Split(data.Channel, ":")
				if len(keys) < 2 {
					fmt.Println("receive unknown channel:", data.Channel)
					return
				}
				switch strings.ToLower(data.Payload) {
				case "hset":
					// err := pool.HMGet(ctx, keys[1], )
					fileds, err := pool.HGetAll(ctx, keys[1]).Result()
					if err != nil {
						fmt.Println("hmget failed:", err)
						return
					}
					fmt.Println("hset data:", fileds)
				default:
					fmt.Println("unknown payload type:", data.Payload)
				}
			}
		}
	}()
}

func publish(ctx context.Context, pool *redis.Client, channel string, val string) {

	// _, err := pool.Publish(ctx, channel, val).Result()
	err := pool.HSet(ctx, channel+val, "hello", "1").Err()
	if err != nil {
		log.Fatal(err)
	}
}
