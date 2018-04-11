'''	Image Feature Locate.

	version 0.1
	2017.11.16
	by JiHan.'''

import os
import time
#Native module
import file_control
import net
import action
import feature_locate
import log


def main():
	ImageN = 0
	Flag = True
	log.init()
	log.info('----program start----')
	while Flag:
		log.info('net is connceting')
		net.net_check()
		log.info('net disconncet')
#		image_path = file_control.window_capture()

	#test image
	#	image_path = os.path.abspath('.\\')+'\\image\\'+'test.png'

		feature1_path = file_control.fetImg_path()
		feature2_path = file_control.fetImg_path('feature1')
#		print('screen image path: '+image_path)
#		print('feature path: '+feature_path)
		try:
			for x in range(1,10):
				xy =  (1,1)
				while xy[0]>0:
					print(u'确定')
					image_path = file_control.window_capture(imgN=ImageN)
					xy = feature_locate.feature_xy(image_path,feature1_path,imgN=ImageN)
					if ImageN<10000:
						ImageN+=1
					else:
						ImageN = 0
		#			print('x:{}'.format(xy[0])+' y:{}'.format(xy[1]))
					action.mouse_action(int(xy[0]),int(xy[1]))
					time.sleep(1)

				print(u'连接')
				time.sleep(2)
				image_path = file_control.window_capture(imgN=ImageN)
				xy = feature_locate.feature_xy(image_path,feature2_path,imgN=ImageN)
				if ImageN<10000:
						ImageN+=1
				else:
					ImageN = 0
	#			print('x:{}'.format(xy[0])+' y:{}'.format(xy[1]))
				action.mouse_action(int(xy[0]),int(xy[1]))
				time.sleep(20)
				if True == net.net_connect_check():
					log.info('net reconnect')
					break
				elif x>8:
					log.debug('reconnect faild!  please check!')
					Flag = False

		except Exception as e:
			print(e)
			Flag = False
			exit()

		pass

	pass


def test():
	ImageN = 0
	feature1_path = file_control.fetImg_path()
	image_path = file_control.window_capture(imgN=ImageN)
	xy = feature_locate.feature_xy(image_path,feature1_path,imgN=ImageN)
	if ImageN<10000:
		ImageN+=1
	else:
		ImageN = 0
	action.mouse_action(int(xy[0]),int(xy[1]))
	pass


if __name__ == '__main__':
	main()
