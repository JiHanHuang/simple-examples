#!/bin/bash

# 端口号
PORT=8999

if [ $# -lt 1 ];then
	#python3 -m http.server "$PORT" --directory "$DIR"
	python -m SimpleHTTPServer "$PORT"
	exit 0
fi
cd $1 || { echo "目录不存在: $1"; exit 1; }
echo "$1"
python -m SimpleHTTPServer "$PORT"

