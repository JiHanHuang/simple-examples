#coding=UTF-8
# -*- coding=UTF-8 -*-

import socket
import socks
import select
import requests
from socketserver import StreamRequestHandler, ThreadingTCPServer
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from base64 import b64encode, b64decode

s_host = '127.0.0.1'
s_port = 9011
buff_len = 2048

AES_ENABLE = False
AES_KEY = b'Sixteen byte key'
IV = b'Sixteen bytes IV'



class SocksClient(StreamRequestHandler):
    def handle(self):
        print('handle...')
        
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((s_host, s_port))
        except Exception as err:
            print(err)
        
        while True:
            r, w, e = select.select([self.connection, s], [], [])
            if self.connection in r:
                buff = self.connection.recv(buff_len)
                encrypt_buff = self.aes_encrypt(buff)
                #print(type(buff),type(encrypt_buff))
                #print('client send:',buff,'   ',encrypt_buff)
                if s.send(encrypt_buff) <= 0:
                    break
                
            if s in r:  
                try:
                    buff = s.recv(buff_len)
                except Exception as err:
                    print('recv exception:',err)               
                decrypt_buff = self.aes_decrypt(buff)
                #print(type(buff),type(decrypt_buff))
                #print('server send:',buff,'   ',decrypt_buff)
                if self.connection.send(decrypt_buff) <= 0:
                    break

        s.close()
        
    def aes_encrypt(self, input):
        if AES_ENABLE and input != None:
            try:
                cipher = AES.new(AES_KEY, AES.MODE_CBC,IV)
                ct_bytes = cipher.encrypt(pad(input, AES.block_size))
                ct = b64encode(ct_bytes).decode('utf-8')
                #print ('encrypt msg:',ct,len(ct))
                return str.encode(ct)
            except (ValueError, KeyError) as e:
                print("Incorrect decryption :",e)
                return None
        else:
            return input
    def aes_decrypt(self, input):
        if AES_ENABLE and input != None:
            try:
                ct = b64decode(input)
                cipher = AES.new(AES_KEY, AES.MODE_CBC, IV)
                pt = unpad(cipher.decrypt(ct), AES.block_size)
                #print("decrypt msg: ", pt,len(pt))
                return pt
            except (ValueError, KeyError) as e:
                print("Incorrect decryption :",e)
                return None
        else:
            return input
        
if __name__ == '__main__':
    print('bind: 127.0.0.1', 9898)
    
    
    print('connect:'+s_host+':',s_port)
    # 使用socketserver库的多线程服务器ThreadingTCPServer启动代理
    with ThreadingTCPServer(('127.0.0.1', 9898), SocksClient) as server:
        server.serve_forever()
        
    print('close server connect!')
    
    
    
    