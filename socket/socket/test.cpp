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

//�����׽���
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
		//�����׽���
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd < 0){ //�����׽���ʧ��
			std::cerr << "socket error" << std::endl;
			return false;
		}

		std::cout << "socket create success, sockfd: " << _sockfd << std::endl;
		
		return true;
	}
	~UdpServer()
	{};
private:
	int _port; //�˿ں�
	std::string _ip; //IP��ַ
	int _sockfd; //�ļ�������
};

#include "udp_server.hpp"

int main()
{
	UdpServer* svr = new UdpServer();
	svr->InitServer();
	return 0;
}

//��
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

//�ַ���IPת����IP
in_addr_t inet_addr(const char *cp);

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

