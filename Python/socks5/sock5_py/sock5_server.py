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

AES_ENABLE = False
AES_KEY = b'Sixteen byte key'
IV = b'Sixteen bytes IV'


class SocksProxy(StreamRequestHandler):
    username = 'admin'
    password = 'admin@1234'
    def handle(self):
        logging.info('Accepting connection from %s:%s' % self.client_address)
        # 协商
        # 从客户端读取并解包两个字节的数据
        header = self.decrypt_recv(2)
        version, nmethods = struct.unpack("!BB", header)
        # 设置socks5协议，METHODS字段的数目大于0
        assert version == SOCKS_VERSION
        assert nmethods > 0
        # 接受支持的方法
        methods = self.get_available_methods(nmethods)
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
        version, cmd, _, address_type = struct.unpack("!BBBB", self.decrypt_recv(4))
        assert version == SOCKS_VERSION
        if address_type == 1:  # IPv4
            address = socket.inet_ntoa(self.decrypt_recv(4))
        elif address_type == 3:  # 域名
            domain_length = self.decrypt_recv(1)[0]
            address = self.decrypt_recv(domain_length)
        elif address_type == 4: # IPv6
            addr_ip = self.decrypt_recv(16)
            address = socket.inet_ntop(socket.AF_INET6, addr_ip)
        else:
            self.server.close_request(self.request)
            return
        port = struct.unpack('!H', self.decrypt_recv(2))[0]
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
    def get_available_methods(self, n):
        methods = []
        for i in range(n):
            methods.append(ord(self.decrypt_recv(1)))
        return methods
    def verify_credentials(self):
        """校验用户名和密码"""
        version = ord(self.decrypt_recv(1))
        assert version == 1
        username_len = ord(self.decrypt_recv(1))
        username = self.decrypt_recv(username_len).decode('utf-8')
        password_len = ord(self.decrypt_recv(1))
        password = self.decrypt_recv(password_len).decode('utf-8')
        if username == self.username and password == self.password:
            # 验证成功, status = 0
            response = struct.pack("!BB", version, 0)
            self.encrypt_sendall(response)
            return True
        # 验证失败, status != 0
        response = struct.pack("!BB", version, 0xFF)
        self.encrypt_sendall(response)
        self.server.close_request(self.request)
        return False
        
    def generate_failed_reply(self, address_type, error_number):
        return struct.pack("!BBBBIH", SOCKS_VERSION, error_number, 0, address_type, 0, 0)
    def exchange_loop(self, client, remote):
        while True:
            # 等待数据
            r, w, e = select.select([client, remote], [], [])
            if client in r:
                data = client.recv(4096)
                data = self.aes_decrypt(data)
                if remote.send(data) <= 0:
                    break
            if remote in r:
                data = remote.recv(4096)
                data = self.aes_encrypt(data)
                if client.send(data) <= 0:
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
        if AES_ENABLE:
            if size < 24 :
                recv_len = 24
            else:
                recv_len = int(size*4.0/3)+1
            #print ('recv len:',recv_len)
            recvmsg = self.connection.recv(recv_len)
            ret = self.aes_decrypt(recvmsg)
            ret = ret[0:size]
        else:
            recvmsg = self.connection.recv(size)
            ret = self.aes_decrypt(recvmsg)
        #print('client msg:',recvmsg,'   ',ret)
        return ret
    def encrypt_sendall(self, bytesdata):
        sendmsg = self.aes_encrypt(bytesdata)
        #print('remote msg:',bytesdata,'     ',sendmsg)
        self.connection.sendall(sendmsg)
        
if __name__ == '__main__':
    print(('127.0.0.1', 9011))
    # 使用socketserver库的多线程服务器ThreadingTCPServer启动代理
    with ThreadingTCPServer(('127.0.0.1', 9011), SocksProxy) as server:
        server.serve_forever()