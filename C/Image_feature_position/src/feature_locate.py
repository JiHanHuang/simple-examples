from PIL import Image
import file_control

maxsim = 10000
split_step = 3
BaseSim = 20
imagN = 0
def feature_xy(srcImgPath='',fetImgPath='',imgN=0):
	global imagN
	imagN = imgN
	if 0 == len(srcImgPath) or 0 == len(fetImgPath):
		raise Exception('Image path error')
	else:
		sImg = Image.open(srcImgPath)
		fImg = Image.open(fetImgPath)
#		sImg.show()
#		fImg.show()
	return cal_xy(sImg,fImg)
	pass

def cal_xy(sImg,fImg):
	global imagN
	x = 0
	y = 0
	sim = maxsim
	fsim = maxsim
	num = 0
	final=0
	sSize = sImg.size
	fSize = fImg.size
	print('Image size:',sSize,fSize)
	for iy in range(0,int(sSize[1] - fSize[1]/split_step),int(fSize[1]/split_step)):
		for ix in range(0,int(sSize[0] - fSize[0]/split_step),int(fSize[0]/split_step)):
			num += 1
			box = [ix,iy,ix+fSize[0],iy+fSize[1]] #left up right down
			img1 = sImg.crop(box)
#			print(box)
			sim = similarity(img1,fImg)
#			print(sim)
			if sim < fsim:
				fsim = sim
				x = ix+fSize[0]/3
				y = iy+fSize[1]/3
				final  = num
				imgfinal = img1
#			img1.show()
#			file_control.image_save(img1,'png','img{}'.format(num))
			pass
		pass
	print(final,' sim: ',fsim)
	if imagN > 10000:
		imagN = 0
		pass
	if fsim < BaseSim:
		#file_control.image_save(imgfinal,'png','target')
		file_control.image_save(imgfinal,'png','target{}'.format(imagN))
		pass
	else:
		file_control.image_save(imgfinal,'png','target_{}-{}'.format(imagN,fsim))
		x = -1
		y = -1
#	imgfinal.show()
	return (x,y)
	pass

def similarity(img1,img2):
	prob = maxsim
	list1 = img1.histogram()
	list2 = img2.histogram()
#	print(list1)
	if len(list1)>len(list2):
		num = len(list2)
#		listdiff = list2[:]
	else:
		num = len(list1)
#		listdiff = list1[:]
	for i in range(0,num):
		prob = abs(list1[i] - list2[i]) + prob
#	print(num)
#	print(listdiff)
#	print(cmp(list1,list2))
	prob = prob/num
	return prob
	pass
