import os
from PIL import  ImageGrab

Image_name = 'image'
Image_type = 'png'
defPath = os.path.abspath('.\\image')

def window_capture(dpath=defPath, imgN=0):
	dpath = dpath+'\\'+Image_name+'{}.'.format(imgN)+Image_type
	im = ImageGrab.grab()
	im.save(dpath,Image_type)
	#print(dpath)
	return dpath
	pass

def fetImg_path(name='feature'):
	path = defPath+'\\'+name+'.'+Image_type
	return path
	pass

def image_save(img,type,name='1',path=defPath):
	path = path+'\\'+name+'.'+type
	img.save(path,type)
	pass
