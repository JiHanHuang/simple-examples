import os
import time
import log
#from timeit import timeit

dec_adress = 'www.baidu.com'

def net_connect_check():
	ping_com = 'ping ' + dec_adress
	exit_code = os.system(ping_com)
	if exit_code:
#		raise Exception('net connect failed')
		print('net connect failed')
		return False
	else:
#		print('net connect sucess')
		return True
	pass

def net_check(t=20,enable=True):
	num = 0
	while enable:
		if False == net_connect_check():
			return False
			break
		else:
			continue
			
		time.sleep(t)

		num += 1
		if num > 10:
			num = 0
			log.info('net connect normal')

	return False
	pass
