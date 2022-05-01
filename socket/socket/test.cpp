#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);


int socket(int domain, int type, int protocol);

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int listen(int sockfd, int backlog);

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

//创建套接字
int socket(int domain, int type, int protocol);

#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

#define DEFAULT 8081

class UdpServer
{
public:
	UdpServer(int port = DEFAULT)
		: _port(port)
		, _sockfd(-1)
	{};
	bool InitServer()
	{
		//创建套接字
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd < 0){ //创建套接字失败
			std::cerr << "socket error" << std::endl;
			return false;
		}

		std::cout << "socket create success, sockfd: " << _sockfd << std::endl;
		
		return true;
	}
	~UdpServer()
	{};
private:
	int _port; //端口号
	std::string _ip; //IP地址
	int _sockfd; //文件描述符
};

#include "udp_server.hpp"

int main()
{
	UdpServer* svr = new UdpServer();
	svr->InitServer();
	return 0;
}

//绑定
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

//字符串IP转整数IP
in_addr_t inet_addr(const char *cp);

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);



#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT 8081

//INADDR_ANY
class UdpServer
{
public:
	//UdpServer(std::string ip, int port = DEFAULT)
	//    :_port(port)
	//    ,_sockfd(-1)
	//    ,_ip(ip)
	//{};
	UdpServer(int port = DEFAULT)
		:_port(port)
		, _sockfd(-1)
	{};
	bool InitServer()
	{
		//创建套接字
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd < 0){ //创建套接字失败
			std::cerr << "socket error" << std::endl;
			return false;
		}

		std::cout << "socket create success, sockfd: " << _sockfd << std::endl;

		//填充网络通信相关信息
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		//local.sin_addr.s_addr = inet_addr(_ip.c_str());
		local.sin_addr.s_addr = INADDR_ANY;

		//绑定
		if (bind(_sockfd, (struct sockaddr*)&local, sizeof(sockaddr)) < 0){ //绑定失败
			std::cerr << "bind error" << std::endl;
			return false;
		}

		std::cout << "bind success" << std::endl;
		return true;
	}
	void Start()
	{
#define SIZE 128
		char buffer[SIZE];
		for (;;){
			struct sockaddr_in peer;
			socklen_t len = sizeof(peer);
			ssize_t size = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
			if (size > 0){
				buffer[size] = '\0';
				int port = ntohs(peer.sin_port);
				std::string ip = inet_ntoa(peer.sin_addr);
				std::cout << ip << ":" << port << "# " << buffer << std::endl;
			}
			else{
				std::cerr << "recvfrom error" << std::endl;
			}
		}
	}

	~UdpServer()
	{
		if (_sockfd >= 0){
			close(_sockfd);
		}
	};
private:
	int _port; //端口号
	int _sockfd; //文件描述符
	//std::string _ip; //IP地址
};


#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//INADDR_ANY
class UdpServer
{
public:
	UdpServer(std::string ip, int port)
		:_sockfd(-1)
		,_port(port)
		,_ip(ip)
	{};
	bool InitServer()
	{
		//创建套接字
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd < 0){ //创建套接字失败
			std::cerr << "socket error" << std::endl;
			return false;
		}

		std::cout << "socket create success, sockfd: " << _sockfd << std::endl;

		//填充网络通信相关信息
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = inet_addr(_ip.c_str());
		//local.sin_addr.s_addr = INADDR_ANY;                                                                                                                                                                                                                                                                                                                                                                                                             

		//绑定
		if (bind(_sockfd, (struct sockaddr*)&local, sizeof(sockaddr)) < 0){ //绑定失败
			std::cerr << "bind error" << std::endl;
			return false;
		}

		std::cout << "bind success" << std::endl;

		return true;
	}
	void Start()
	{
#define SIZE 128
		char buffer[SIZE];
		for (;;){
			struct sockaddr_in peer;
			socklen_t len = sizeof(peer);
			ssize_t size = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
			if (size > 0){
				buffer[size] = '\0';
				int port = ntohs(peer.sin_port);
				std::string ip = inet_ntoa(peer.sin_addr);
				std::cout << ip << ":" << port << "# " << buffer << std::endl;
			}
			else{
				std::cerr << "recvfrom error" << std::endl;
			}
		}
	}
	~UdpServer()
	{
		if (_sockfd >= 0){
			close(_sockfd);
		}
	};
private:
	int _sockfd; //文件描述符
	int _port; //端口号
	std::string _ip; //IP地址
};

#include "udp_server.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2){
		std::cerr << "Usage: " << argv[0] << " port" << std::endl;
		return 1;
	}
	std::string ip = "127.0.0.1"; //本地环回
	//std::string ip = "49.232.66.206"; //公网IP
	int port = atoi(argv[1]);
	UdpServer* svr = new UdpServer(ip, port);
	svr->InitServer();
	svr->Start();
	return 0;
}


#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class UdpClient
{
public:
	UdpClient(std::string server_ip, int server_port)
		:_sockfd(-1)
		,_server_port(server_port)
		,_server_ip(server_ip)
	{}
	bool InitClient()
	{
		//创建套接字
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd < 0){
			std::cerr << "socket create error" << std::endl;
			return false;
		}
		return true;
	}
	void Start()
	{
		std::string msg;
		struct sockaddr_in peer;
		memset(&peer, '\0', sizeof(peer));
		peer.sin_family = AF_INET;
		peer.sin_port = htons(_server_port);
		peer.sin_addr.s_addr = inet_addr(_server_ip.c_str());

		for (;;){
			std::cout << "Please Enter# ";
			getline(std::cin, msg);
			sendto(_sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr*)&peer, sizeof(peer));
		}
	}
	~UdpClient()
	{
		if (_sockfd >= 0){
			close(_sockfd);
		}
	}
private:
	int _sockfd; //文件描述符
	int _server_port; //服务端端口号
	std::string _server_ip; //服务端IP地址
};

#include "udp_client.hpp"


int main(int argc, char* argv[])
{
	if (argc != 3){
		std::cerr << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
		return 1;
	}
	std::string server_ip = argv[1];
	int server_port = atoi(argv[2]);
	UdpClient* clt = new UdpClient(server_ip, server_port);
	clt->InitClient();
	clt->Start();
	return 0;
}

void Start()
{
#define SIZE 128
	char buffer[SIZE];
	for (;;){
		struct sockaddr_in peer;
		socklen_t len = sizeof(peer);
		ssize_t size = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
		if (size > 0){
			buffer[size] = '\0';
			int port = ntohs(peer.sin_port);
			std::string ip = inet_ntoa(peer.sin_addr);
			std::cout << ip << ":" << port << "# " << buffer << std::endl;
		}
		else{
			std::cerr << "recvfrom error" << std::endl;
		}

		std::string echo_msg = "server get!->";
		echo_msg += buffer;
		sendto(_sockfd, echo_msg.c_str(), echo_msg.size(), 0, (struct sockaddr*)&peer, len);
	}
}


void Start()
{
	std::string msg;
	struct sockaddr_in peer;
	memset(&peer, '\0', sizeof(peer));
	peer.sin_family = AF_INET;
	peer.sin_port = htons(_server_port);
	peer.sin_addr.s_addr = inet_addr(_server_ip.c_str());

	for (;;){
		std::cout << "Please Enter# ";
		getline(std::cin, msg);
		sendto(_sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr*)&peer, sizeof(peer));
		
#define SIZE 128
		char buffer[SIZE];
		struct sockaddr_in tmp;
		socklen_t len = sizeof(tmp);
		ssize_t size = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&tmp, &len);
		if (size > 0){
			buffer[size] = '\0';
			std::cout << buffer << std::endl;
		}
	}
}
