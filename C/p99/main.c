/**
 * @file main.c 基于libevent编写的并发通用测试程序，用于P99时延测试
 * @author huangjiahao (huangjh110@chinatelecom.cn)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025 中电信智能网络科技有限公司
 * 
 */

#include "tracker.h"
#include <event2/event.h>
#include <getopt.h>
#include <hiredis/adapters/libevent.h>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct event_base *g_base = NULL;
static long g_total_commands = 0;
static long g_executed_commands = 0;
static long start_time = 0, end_time = 0;
static int g_connections = 0;

void print_results() {
    end_time = current_time_us();
    double total_time_seconds =
        (end_time - start_time) / 1000000.0;              // 总时间（秒）
    int minutes = (int)(total_time_seconds / 60);         // 转换为分钟
    double seconds = total_time_seconds - (minutes * 60); // 剩余的秒数

    printf("------------------------------------\n");
    printf("Total time taken: %dm%.1fs\n", minutes, seconds);
    printf("Total commands executed: %ld\n", g_executed_commands);
    double commands_per_second = g_executed_commands / total_time_seconds;
    printf("Commands executed per second: %.0f\n", commands_per_second);
    tracker_print_results();
}

typedef struct {
    redisAsyncContext *redis;
    struct event_base *base;
    long start_time;
} client_t;

void get_callback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    client_t *client = privdata;

    if (reply == NULL)
        return;

    long end_time = current_time_us();
    long latency = end_time - client->start_time;
    tracker_insert(latency);

    g_executed_commands++;
    if (g_executed_commands >= g_total_commands) {
        print_results();
        if (g_base) {
            event_base_loopbreak(g_base);
        }
        return;
    }

    client->start_time = current_time_us();
    redisAsyncCommand(client->redis, get_callback, client, "GET test");
}

void connect_callback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    g_connections--;
    // printf("Connected %d\n", g_connections);
    if (g_connections == 0) {
        printf("All clients connected\n");
    }
}

void disconnect_callback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Disconnected: %s\n", c->errstr);
    }
}

client_t *start_client(struct event_base *base, const char *host, int port) {
    client_t *client = calloc(1, sizeof(client_t));

    redisAsyncContext *c = redisAsyncConnect(host, port);
    if (c->err) {
        printf("Connection error: %s\n", c->errstr);
        exit(1);
    }

    client->redis = c;
    client->base = base;
    redisLibeventAttach(c, base);
    redisAsyncSetConnectCallback(c, connect_callback);
    redisAsyncSetDisconnectCallback(c, disconnect_callback);

    client->start_time = current_time_us();
    redisAsyncCommand(c, get_callback, client, "GET p99");
    return client;
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nCaught SIGINT, printing results...\n");
        print_results();
        if (g_base) {
            event_base_loopbreak(g_base);
        }
    }
}

int main(int argc, char **argv) {
    const char *host = "127.0.0.1"; // 默认值
    int port = 6379;                // 默认值
    int connections = 8;            // 默认值
    g_total_commands = 1000000;     // 默认值

    int opt;
    while ((opt = getopt(argc, argv, "a:p:c:n:")) != -1) {
        switch (opt) {
        case 'a':
            host = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'c':
            connections = atoi(optarg);
            break;
        case 'n':
            g_total_commands = atol(optarg);
            if (g_total_commands <= connections || g_total_commands <= 0) {
                fprintf(stderr, "Total commands (-n) must be greater than 0 "
                                "and less than connections(-c)\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr,
                    "Usage: %s [-a <redis_host>] [-p <redis_port>] [-c "
                    "<connections>] [-n <total_commands>]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // 打印解析后的参数值
    printf("Parsed Parameters:\n");
    printf("Redis Host: %s\n", host);
    printf("Redis Port: %d\n", port);
    printf("Connections: %d\n", connections);
    printf("Total Commands: %ld\n", g_total_commands);

    g_connections = connections;

    double percentiles[] = {0.5, 0.95, 0.9, 0.99, 0.999, 0.9999};
    int percentile_count = sizeof(percentiles) / sizeof(double);
    tracker_init(percentiles, percentile_count);

    signal(SIGINT, signal_handler);

    g_base = event_base_new();

    start_time = current_time_us();
    client_t **client_list = calloc(connections, sizeof(client_t *));
    for (int i = 0; i < connections; i++) {
        client_list[i] = start_client(g_base, host, port);
    }

    event_base_dispatch(g_base);

    return 0;
}