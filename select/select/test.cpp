int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);      //���������������set�����fd��λ
int  FD_ISSET(int fd, fd_set *set);    //����������������set�����fd��λ�Ƿ�Ϊ��
void FD_SET(int fd, fd_set *set);      //����������������set�����fd��λ
void FD_ZERO(fd_set *set);             //���������������set��ȫ��λ

fd_set readset;
FD_SET(fd, &readset);
select(fd + 1, &readset, NULL, NULL, NULL);
if (FD_ISSET(fd, readset)){...}


#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>

class Socket{
public:
	//�����׽���
	static int SocketCreate()
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//���ö˿ڸ���
		int opt = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		return sock;
	}
	//��
	static void SocketBind(int sock, int port)
	{
		struct sockaddr_in local;
		memset(&local, 0, sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(port);
		local.sin_addr.s_addr = INADDR_ANY;
		
		socklen_t len = sizeof(local);
			36
		if (bind(sock, (struct sockaddr*)&local, len) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
	}
	//����
	static void SocketListen(int sock, int backlog)
	{
		if (listen(sock, backlog) < 0){
			std::cerr << "listen error" << std::endl;
			exit(4);
		}
	}
};

#pragma once

#include "socket.hpp"
#include <sys/select.h>

#define BACK_LOG 5
#define NUM 1024
#define DFL_FD - 1

class SelectServer{
private:
	int _listen_sock; //�����׽���
	int _port; //�˿ں�
public:
	SelectServer(int port)
		: _port(port)
	{}
	void InitSelectServer()
	{
		_listen_sock = Socket::SocketCreate();
		Socket::SocketBind(_listen_sock, _port);
		Socket::SocketListen(_listen_sock, BACK_LOG);
	}
	void Run()
	{
		fd_set readfds; //���ļ���������
		int fd_array[NUM]; //������Ҫ�����Ӷ��¼��Ƿ�������ļ�������
		ClearFdArray(fd_array, NUM, DFL_FD); //�������е�����λ������Ϊ��Ч
		fd_array[0] = _listen_sock; //�������׽�����ӵ�fd_array�����еĵ�0��λ��
		for (;;){
			FD_ZERO(&readfds); //���readfds
			//��fd_array���鵱�е��ļ���������ӵ�readfds���У�����¼�����ļ�������
			int maxfd = DFL_FD;
			for (int i = 0; i < NUM; i++){
				if (fd_array[i] == DFL_FD) //������Ч��λ��
					continue;
				FD_SET(fd_array[i], &readfds); //����Чλ�õ��ļ���������ӵ�readfds����
				if (fd_array[i] > maxfd) //��������ļ�������
					maxfd = fd_array[i];
			}
			struct timeval timeout = { 5, 0 }; //ÿ��5��timeoutһ��
			switch (select(maxfd + 1, &readfds, nullptr, nullptr, /*&timeout*/nullptr)){
				case 0:
					std::cout << "timeout: " << timeout.tv_sec << "." << timeout.tv_usec << std::endl;
					break;
				case -1:
					std::cerr << "select error" << std::endl;
					//std::cout<<errno<<std::endl;
					break;
				default:
					//�������¼�����
					//std::cout<<"���¼�����... timeout: "<<timeout.tv_sec<<"."<<timeout.tv_usec<<std::endl;
					HandlerEvent(readfds, fd_array, NUM);
					break;
			}//end switch
		}//end for
	}
	void HandlerEvent(const fd_set& readfds, int fd_array[], int num)
	{
		for (int i = 0; i < num; i++){
			if (fd_array[i] == DFL_FD){ //������Ч��λ��
				continue;
			}
			if (fd_array[i] == _listen_sock&&FD_ISSET(fd_array[i], &readfds)){ //�����¼�����
				//��ȡ����
				struct sockaddr_in peer;
				memset(&peer, 0, sizeof(peer));
				socklen_t len = sizeof(peer);
				int sock = accept(_listen_sock, (struct sockaddr*)&peer, &len);
				if (sock < 0){ //��ȡ����ʧ��
					std::cerr << "accept error" << std::endl;
					continue;
				}
				std::string peer_ip = inet_ntoa(peer.sin_addr);
				int peer_port = ntohs(peer.sin_port);
				std::cout << "get a new link[" << peer_ip << ":" << peer_port << "]" << std::endl;

				if (!SetFdArray(fd_array, num, sock)){ //����ȡ�����׽�����ӵ�fd_array����
					close(sock);
					std::cout << "select server is full, close fd: " << sock << std::endl;
				}
			}
			else if (FD_ISSET(fd_array[i], &readfds)){ //���¼�����
				char buffer[1024];
				ssize_t size = read(fd_array[i], buffer, sizeof(buffer)-1);
				if (size > 0){ //��ȡ�ɹ�
					buffer[size] = '\0';
					std::cout << "echo# " << buffer << std::endl;
				}
				else if (size == 0){ //�Զ����ӹر�
					std::cout << "client quit" << std::endl;
					close(fd_array[i]);
					fd_array[i] = DFL_FD; //�����ļ���������fd_array�����
				}
				else{
					std::cerr << "read error" << std::endl;
					close(fd_array[i]);
					fd_array[i] = DFL_FD; //�����ļ���������fd_array�����
				}
			}
		}
	}
	~SelectServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	void ClearFdArray(int fd_array[], int num, int default_fd)
	{
		for (int i = 0; i < num; i++){
			fd_array[i] = default_fd;
		}
	}
	bool SetFdArray(int fd_array[], int num, int fd)
	{
		for (int i = 0; i <num; i++){
			if (fd_array[i] == DFL_FD){ //��λ��û�б�ʹ��
				fd_array[i] = fd;
				return true;
			}
		}
		return false; //fd_array��������
	}
};


struct timeval timeout = { 0, 0 }; //ÿ��0��timeoutһ��
switch (select(maxfd + 1, &readfds, nullptr, nullptr, &timeout)){
	case 0:
		std::cout << "timeout..." << std::endl;
		break;
	case -1:
		std::cerr << "select error" << std::endl;                                   
		break;
	default:
		//�������¼�����                                                          
		std::cout << "���¼�����..." << std::endl;
		//HandlerEvent(readfds, fd_array, NUM);                                   
		break;
}

struct timeval timeout = { 5, 0 }; //ÿ��5��timeoutһ��
switch (select(maxfd + 1, &readfds, nullptr, nullptr, &timeout)){
case 0:
	std::cout << "timeout: " << timeout.tv_sec << std::endl;
	break;
case -1:
	std::cerr << "select error" << std::endl;
	break;
default:
	//�������¼�����                                                          
	std::cout << "���¼�����... timeout: " << timeout.tv_sec << std::endl;
	//HandlerEvent(readfds, fd_array, NUM);                                   
	break;
}

#include "select_server.hpp"
#include <string>

static void Usage(std::string proc)
{
	std::cerr << "Usage: " << proc << " port" << std::endl;
}
int main(int argc, char* argv[])
{
	if (argc != 2){
		Usage(argv[0]);
		exit(1);
	}
	int port = atoi(argv[1]);

	SelectServer* svr = new SelectServer(port);
	svr->InitSelectServer();
	svr->Run();
	
	return 0;
}


#include <iostream>
#include <sys/types.h>

int main()
{
	std::cout << sizeof(fd_set)* 8 << std::endl;
	return 0;
}

#define NUM (sizeof(fd_set)*8)
