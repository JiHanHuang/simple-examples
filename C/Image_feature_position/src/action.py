import win32api
import win32con
import win32gui
import time
from ctypes import *

def mouse_action(x,y,act=0):
	print('mouse_action',x,y)
	if x<0 or y<0:
		return False
	mouse_move(x,y)
	time.sleep(0.5)
	mouse_act(act)
	time.sleep(0.5)
	mouse_move(0,0)
	print('mouse_action over')
	return True
	pass

def mouse_move(x,y):
	windll.user32.SetCursorPos(x,y)
	pass

def mouse_act(act):
	if act == 0:
		win32api.mouse_event(win32con.MOUSEEVENTF_LEFTDOWN,0,0,0,0)
		win32api.mouse_event(win32con.MOUSEEVENTF_LEFTUP,0,0,0,0)
	elif act == 1:
		pass
	else:
		pass
	pass
