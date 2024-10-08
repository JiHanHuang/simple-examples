#!/bin/bash

# 检查是否提供了必要的参数
if [ $# -ne 2 ]; then
    echo "Usage: $0 <host> <port>"
    exit 1
fi

export LD_LIBRARY_PATH=./

# 从命令行参数获取地址和端口
HOST=$1
PORT=$2

# 间隔时间（秒）
INTERVAL=5

# 日志文件
LOGFILE="/var/log/solar/nfm/tcp_check.log"

while true; do
    # 获取当前时间
    TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
    
    echo -n "$TIMESTAMP - " >> $LOGFILE
    # 使用nc工具探测TCP连接
    nc -z -v -w 1 $HOST $PORT >> $LOGFILE 2>&1
    RESULT=$?
    
    if [ $RESULT -ne 0 ]; then
        echo "ERR >>> $TIMESTAMP - Connection to $HOST $PORT failed" >> $LOGFILE
    fi
    
    # 等待指定的间隔时间
    sleep $INTERVAL
done

