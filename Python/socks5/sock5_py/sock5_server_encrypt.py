import logging
import select
import socket
import struct
from socketserver import StreamRequestHandler, ThreadingTCPServer
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from base64 import b64encode, b64decode


logging.basicConfig(level=logging.DEBUG)
SOCKS_VERSION = 5
buff_block = 4096

AES_ENABLE = True
AES_KEY = b'Sixteen byte key'
IV = b'Sixteen bytes IV'


class SocksProxy(StreamRequestHandler):
    username = 'admin'
    password = 'admin@1234'
    def handle(self):
        logging.info('Accepting connection from %s:%s' % self.client_address)
        # 协商
        # 从客户端读取并解包两个字节的数据
        header = self.decrypt_recv(buff_block)
        version, nmethods = struct.unpack("!BB", header[0:2])
        # 设置socks5协议，METHODS字段的数目大于0
        assert version == SOCKS_VERSION
        assert nmethods > 0
        # 接受支持的方法
        methods = self.get_available_methods(nmethods,header)
        # 检查是否支持用户名/密码认证方式
        print(methods)
        if 2 not in set(methods):
            #self.server.close_request(self.request)
            # 发送协商响应数据包
            self.encrypt_sendall(struct.pack("!BB", SOCKS_VERSION, 0))
        else:
            # 发送协商响应数据包
            self.encrypt_sendall(struct.pack("!BB", SOCKS_VERSION, 2))
            # 校验用户名和密码
            if not self.verify_credentials():
                return
        # 请求
        recvmsg_buff = self.decrypt_recv(buff_block)
        version, cmd, _, address_type = struct.unpack("!BBBB", recvmsg_buff[0:4])
        assert version == SOCKS_VERSION
        addressmsg = recvmsg_buff[4:]
        if address_type == 1:  # IPv4
            address = socket.inet_ntoa(addressmsg[0:4])
            mark = 4
        elif address_type == 3:  # 域名
            domain_length = addressmsg[0]
            address = addressmsg[1:domain_length+1]
            mark = domain_length+1
        elif address_type == 4: # IPv6
            addr_ip = addressmsg[0:16]
            address = socket.inet_ntop(socket.AF_INET6, addr_ip)
            mark = 16
        else:
            self.server.close_request(self.request)
            return
        port = struct.unpack('!H', addressmsg[mark:mark+2])[0]
        # 响应，只支持CONNECT请求
        remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print(address_type,'remote address:',address,port)
        try:
            if cmd == 1:  # CONNECT
                remote.connect((address, port))
                bind_address = remote.getsockname()
                logging.info('Connected to %s %s' % (address, port))
            else:
                self.server.close_request(self.request)
            addr = struct.unpack("!I", socket.inet_aton(bind_address[0]))[0]
            port = bind_address[1]
            reply = struct.pack("!BBBBIH", SOCKS_VERSION, 0, 0, 1, addr, port)
        except Exception as err:
            logging.error('remote connect failed:',err)
            # 响应拒绝连接的错误
            reply = self.generate_failed_reply(address_type, 5)
            remote.close()
            return
        self.encrypt_sendall(reply)
        # 建立连接成功，开始交换数据
        if reply[1] == 0 and cmd == 1:
            self.exchange_loop(self.connection, remote)
        remote.close()
        self.server.close_request(self.request)
    def get_available_methods(self, n,header):
        methods = []
        for i in range(n):
            c = header[i+2]
            #print(c,type(c))
            #methods.append(ord(c))
            methods.append(c)
        return methods
    def verify_credentials(self):
        """校验用户名和密码"""
        recvmsg = self.decrypt_recv(buff_block)
        version = recvmsg[0]
        assert version == 1
        username_len = recvmsg[1]
        username = recvmsg[2:username_len+2].decode('utf-8')
        password_len = recvmsg[username_len+2]
        password = recvmsg[username_len+3:username_len+3+password_len].decode('utf-8')
        #print(username,password)
        if username == self.username and password == self.password:
            # 验证成功, status = 0
            response = struct.pack("!BB", version, 0)
            self.encrypt_sendall(response)
            print('auth success!')
            return True
        # 验证失败, status != 0
        response = struct.pack("!BB", version, 0xFF)
        self.encrypt_sendall(response)
        self.server.close_request(self.request)
        print('auth failed!')
        return False
        
    def generate_failed_reply(self, address_type, error_number):
        return struct.pack("!BBBBIH", SOCKS_VERSION, error_number, 0, address_type, 0, 0)
    def exchange_loop(self, client, remote):
        while True:
            # 等待数据
            r, w, e = select.select([client, remote], [], [])
            if client in r:
                try:
                    data = client.recv(4)
                    buff_len = int.from_bytes(data, byteorder='little', signed=True)
                    data = client.recv(buff_len)
                    data = self.aes_decrypt(data)
                
                    if remote.send(data) <= 0:
                        break
                except Exception as err:
                    print('server send to remote failed: ',err)
                    break  
            if remote in r:
                try:
                    data = remote.recv(buff_block)
                    data = self.aes_encrypt(data)
                    buff_len = len(data)
                    lenbyte = buff_len.to_bytes(4, byteorder='little', signed=True)
 
                    if client.send(lenbyte) <= 0:
                        break
                    if client.send(data) <= 0:
                        break
                except Exception as err:
                    print('server send to client failed: ',err)
                    break    
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
            
    def decrypt_recv(self, size):
        
        recvmsg = self.connection.recv(4)
        buff_len = int.from_bytes(recvmsg, byteorder='little', signed=True)
        recvmsg = self.connection.recv(buff_len)
        ret = self.aes_decrypt(recvmsg)
        #print('recv client msg:',recvmsg,ret,buff_len)
        return ret
    def encrypt_sendall(self, bytesdata):
        sendmsg = self.aes_encrypt(bytesdata)
        buff_len = len(sendmsg)
        lenbyte = buff_len.to_bytes(4, byteorder='little', signed=True)
        self.connection.sendall(lenbyte)
        #print('send to client msg:',bytesdata,sendmsg,buff_len)
        self.connection.sendall(sendmsg)
        
if __name__ == '__main__':
    print(('127.0.0.1', 9011))
    # 使用socketserver库的多线程服务器ThreadingTCPServer启动代理
    with ThreadingTCPServer(('127.0.0.1', 9011), SocksProxy) as server:
        server.serve_forever()
        #server.handle_request()
        
        
        