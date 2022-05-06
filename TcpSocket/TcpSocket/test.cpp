//���ض��ڴ�����������
void bzero(void *s, size_t n);

//����
int listen(int sockfd, int backlog);

//��ȡ����
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

//���ӷ�����
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

//tcp_server.hpp
#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BACKLOG 5

class TcpServer
{
public:
	TcpServer(int port)
		: _listen_sock(-1)
		, _port(port)
	{}
	void InitServer()
	{
		//�����׽���
		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_listen_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//��
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = INADDR_ANY;
		
		if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
		//����
		if (listen(_listen_sock, BACKLOG) < 0){
			std::cerr << "listen error" << std::endl;
			exit(4);
		}
	}
	void Service(int sock, std::string client_ip, int client_port)
	{
		char buffer[1024];
		while (true){
			ssize_t size = read(sock, buffer, sizeof(buffer)-1);
			if (size > 0){ //��ȡ�ɹ�
				buffer[size] = '\0';
				std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

				write(sock, buffer, size);
			}
			else if (size == 0){ //�Զ˹ر�����
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //��ȡʧ��
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //�黹�ļ�������
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
	void Start()
	{
		for (;;){
			//��ȡ����
			struct sockaddr_in peer;
			memset(&peer, '\0', sizeof(peer));
			socklen_t len = sizeof(peer);
			int sock = accept(_listen_sock, (struct sockaddr*)&peer, &len);
			if (sock < 0){
				std::cerr << "accept error, continue next" << std::endl;
				continue;
			}
			std::string client_ip = inet_ntoa(peer.sin_addr);
			int client_port = ntohs(peer.sin_port);
			std::cout << "get a new link [" << client_ip << "]:" << client_port << std::endl;

			//��������
			Service(sock, client_ip, client_port);
		}
	}
	~TcpServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	int _listen_sock; //�����׽���
	int _port; //�˿ں�
};

