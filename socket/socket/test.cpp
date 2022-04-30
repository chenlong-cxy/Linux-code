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

