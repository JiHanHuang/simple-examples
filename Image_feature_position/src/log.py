import os
#import platform
import logging

Def_path = os.path.abspath('.\\log')

def init(name='log.txt',path=Def_path):
	logfile = path+'\\'+name
	logging.basicConfig(
		level=logging.DEBUG,
		format='%(asctime)s : %(levelname)s : %(message)s',
		filename=logfile,
		filemode='w',
		)
	pass

def debug(str):
	logging.debug(str)
	pass

def info(str):
	logging.info(str)
	pass

def warning(str):
	logging.warning(str)
	pass
