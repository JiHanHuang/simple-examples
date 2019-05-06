import os
import platform
import logging
import time
from pprint import pprint

class log:
	__out_flag=3
	__log_flag=3
	def __init__(self,name=None,path=None,out_level=2,log_level=2):
		if out_level >= 0:
			self.__out_flag = out_level
		if log_level >= 0:
			self.__log_flag = log_level
		if(platform.system() == "Windows"):
			Def_path = os.path.abspath('.\\log')
			if not os.path.exists(Def_path):
				os.makedirs(Def_path)
			split_mark="\\"
		elif(platform.system() == "Linux"):
			Def_path = os.path.abspath('./log')
			if not os.path.exists(Def_path):
				os.makedirs(Def_path)
			split_mark="/"
		else:
			print ("invaild platform: ",platform.system())
			exit()

		if name == None:
			name=""+time.strftime("%Y-%m-%d", time.localtime())+".log"
		else:
			name=name+time.strftime("%Y-%m-%d", time.localtime())+".log"
		if path != None:
			logfile = path+split_mark+name
		else:
			logfile = Def_path+split_mark+name
		if os.path.exists(logfile):
			file_mode='a'
		else:
			file_mode='w'
		logging.basicConfig(
			level=logging.DEBUG,
			format='%(asctime)s : %(levelname)s : %(message)s',
			filename=logfile,
			filemode=file_mode,
			)
		pass


	def debug(self,str,*strs):
		if self.__out_flag == 0:
			print("Debug:")
			print (str)
			for value in strs:
				if type(value) is dict:
					pprint (value)
				else:
					print (value)
		if self.__log_flag == 0:
			logging.debug(str)
			for value in strs:
				logging.debug(value)

	def info(self,str,*strs):
		if self.__out_flag <= 1 :
			print("Info:")
			print (str)
			for value in strs:
				if type(value) is dict:
					pprint (value)
				else:
					print (value)
		if self.__log_flag <= 1:
			logging.info(str)
			for value in strs:
				logging.info(value)

	def warning(self,str,*strs):
		if self.__out_flag <= 2:
			print("Warning:")
			print (str)
			for value in strs:
				if type(value) is dict:
					pprint (value)
				else:
					print (value)
		if self.__log_flag <= 2:
			logging.warning(str)
			for value in strs:
				logging.warning(value)
