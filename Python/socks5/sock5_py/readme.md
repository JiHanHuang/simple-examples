#sock5代理：
##测试平台：win10，python3.7
###文件描述：
	sock5_client.py    不带加密的sock5客户端
	sock5_client_encrypt.py		带加密的sock5客户端
	sock5_server.py	    不带加密的sock5服务端
	sock5_server_encrypt.py	    不带加密的sock5服务端
	
###使用方法：
	配置代理端口，这里所有默认代理ip：127.0.0.1，端口：9898
	不加密：
	先执行服务端代码：python3 sock5_server.py
	再执行客户端代码：python3 sock5_client.py
	加密，方式同上