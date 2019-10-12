import time
import socket
import json
import random
from locust import Locust, TaskSet, events, task

token = 100

class TcpSocketClient(socket.socket):
    # locust tcp client
    # author: Max.Bai@2017
    def __init__(self, af_inet, socket_type):
        super(TcpSocketClient, self).__init__(af_inet, socket_type)

    def connect(self, addr):
        start_time = time.time()
        try:
            super(TcpSocketClient, self).connect(addr)
        except Exception as e:
            total_time = int((time.time() - start_time) * 1000)
            events.request_failure.fire(request_type="tcpsocket", name="connect", response_time=total_time, exception=e)
        else:
            total_time = int((time.time() - start_time) * 1000)
            events.request_success.fire(request_type="tcpsocket", name="connect", response_time=total_time,
                                        response_length=0)
    def send(self, msg):
        start_time = time.time()
        try:
            super(TcpSocketClient, self).send(msg)
        except Exception as e:
            total_time = int((time.time() - start_time) * 1000)
            events.request_failure.fire(request_type="tcpsocket", name="send", response_time=total_time, exception=e)
        else:
            total_time = int((time.time() - start_time) * 1000)
            events.request_success.fire(request_type="tcpsocket", name="send", response_time=total_time,
                                        response_length=0)

    def recv(self, bufsize):
        recv_data = ''
        start_time = time.time()
        try:
            recv_data = super(TcpSocketClient, self).recv(bufsize)
        except Exception as e:
            total_time = int((time.time() - start_time) * 1000)
            events.request_failure.fire(request_type="tcpsocket", name="recv", response_time=total_time, exception=e)
        else:
            total_time = int((time.time() - start_time) * 1000)
            events.request_success.fire(request_type="tcpsocket", name="recv", response_time=total_time,
                                        response_length=0)
        return recv_data

class TcpSocketLocust(Locust):
    """
    This is the abstract Locust class which should be subclassed. It provides an TCP socket client
    that can be used to make TCP socket requests that will be tracked in Locust's statistics.
    author: Max.bai@2017
    """
    def __init__(self, *args, **kwargs):
        super(TcpSocketLocust, self).__init__(*args, **kwargs)
        self.client = TcpSocketClient(socket.AF_INET, socket.SOCK_STREAM)
        ADDR = (self.host, self.port)
        self.client.connect(ADDR)


def user_send_msg():
    send_json = {
        "access_token" : "101",
        "app_uuid" : "par_auth_msg_uuid",
        "client_ip" : "127.0.0.1",
        "sess_id" : 101
    }
#    send_json['access_token'] = str(random.randint(100,5000))
    global token
    if (token > 10000):
        token = 100
    else:
        token += 1
    send_json['access_token'] = str(token)
    send_str = json.dumps(send_json)
    
    value = (len(send_str)).to_bytes(4, byteorder='big')+send_str.encode()
    lenght = (len(value)+4).to_bytes(4, byteorder='big')
    tag = b'\x12\x00\x00\x02'
    byte_str =tag +lenght + value + b'\x00\x00\x00\x00'
    return byte_str

def user_recv_msg(data):
    pass

class user_set(TaskSet):
    @task
    def send_data(self):
        byte_str = user_send_msg()

        #send msg
        self.client.send(byte_str)
        data = self.client.recv(2048)
        user_recv_msg(data)

class TcpTestUser(TcpSocketLocust):
    host = "127.0.0.1"  #host
    port = 43           #port
    #between the execution of tasks (min_wait and max_wait) as well as other user behaviours
    min_wait = 1000
    max_wait = 10000
    
    task_set = user_set


    


if __name__ == "__main__":
    print(user_send_msg())
#    user = TcpTestUser()
#    user.run()
