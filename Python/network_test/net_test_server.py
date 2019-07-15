#!/usr/bin/python
# -*- coding: UTF-8 -
#python2

import time
import socket               # 导入 socket 模块
import sys
 
s = socket.socket()         # 创建 socket 对象
#host = socket.gethostname() # 获取本地主机名
if len(sys.argv) > 1:
	host = sys.argv[1]
else:
	host='0.0.0.0'

if len(sys.argv) > 2:
	port = int(sys.argv[2])
else:
	port = 12345

s.bind((host, port))        # 绑定端口

print 'bind: ',host,port

try:
	while True:
		s.listen(5)                 # 等待客户端连接
		c,addr = s.accept()     # 建立客户端连接
		now = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
		print now,' connect addr: ', c.recv(1024)
		c.send('connect sucess!')
		c.close()                # 关闭连接
except  KeyboardInterrupt as e:
	print ' Stop listen!'
