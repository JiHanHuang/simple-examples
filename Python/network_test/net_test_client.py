#!/usr/bin/python
# -*- coding: UTF-8 -
#python2

import sys
import socket               # 导入 socket 模块
import fcntl
import struct


def get_ip_address(ifname):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	return socket.inet_ntoa(fcntl.ioctl(
		s.fileno(),
		0x8915,  # SIOCGIFADDR
		struct.pack('256s', ifname[:15])
		)[20:24])

def main():
	s = socket.socket()         # 创建 socket 对象
	#host = socket.gethostname() # 获取本地主机名
	#host='0.0.0.0'
	localIP = get_ip_address('eth0')

	if len(sys.argv) > 1:
		host = sys.argv[1]
	else:
		print 'input host!'
		exit()

	if len(sys.argv) > 2:
		port = int(sys.argv[2])
	else:
		port = 12345


	print 'connect: ',host,port,'...'

	try:
		s.connect((host, port))
		s.send(localIP)
		print s.recv(1024)
		s.close()
	except  KeyboardInterrupt:
		print ' Stop listen!'

if __name__ == '__main__':
	main()
