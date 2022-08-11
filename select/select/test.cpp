int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);      //用来清除描述词组set中相关fd的位
int  FD_ISSET(int fd, fd_set *set);    //用来测试描述词组set中相关fd的位是否为真
void FD_SET(int fd, fd_set *set);      //用来设置描述词组set中相关fd的位
void FD_ZERO(fd_set *set);             //用来清除描述词组set的全部位

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
	//创建套接字
	static int SocketCreate()
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//设置端口复用
		int opt = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		return sock;
	}
	//绑定
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
	//监听
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
	int _listen_sock; //监听套接字
	int _port; //端口号
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
		fd_set readfds; //读文件描述符集
		int fd_array[NUM]; //保存需要被监视读事件是否就绪的文件描述符
		ClearFdArray(fd_array, NUM, DFL_FD); //将数组中的所有位置设置为无效
		fd_array[0] = _listen_sock; //将监听套接字添加到fd_array数组中的第0个位置
		for (;;){
			FD_ZERO(&readfds); //清空readfds
			//将fd_array数组当中的文件描述符添加到readfds当中，并记录最大的文件描述符
			int maxfd = DFL_FD;
			for (int i = 0; i < NUM; i++){
				if (fd_array[i] == DFL_FD) //跳过无效的位置
					continue;
				FD_SET(fd_array[i], &readfds); //将有效位置的文件描述符添加到readfds当中
				if (fd_array[i] > maxfd) //更新最大文件描述符
					maxfd = fd_array[i];
			}
			struct timeval timeout = { 5, 0 }; //每隔5秒timeout一次
			switch (select(maxfd + 1, &readfds, nullptr, nullptr, /*&timeout*/nullptr)){
				case 0:
					std::cout << "timeout: " << timeout.tv_sec << "." << timeout.tv_usec << std::endl;
					break;
				case -1:
					std::cerr << "select error" << std::endl;
					//std::cout<<errno<<std::endl;
					break;
				default:
					//正常的事件处理
					//std::cout<<"有事件发生... timeout: "<<timeout.tv_sec<<"."<<timeout.tv_usec<<std::endl;
					HandlerEvent(readfds, fd_array, NUM);
					break;
			}//end switch
		}//end for
	}
	void HandlerEvent(const fd_set& readfds, int fd_array[], int num)
	{
		for (int i = 0; i < num; i++){
			if (fd_array[i] == DFL_FD){ //跳过无效的位置
				continue;
			}
			if (fd_array[i] == _listen_sock&&FD_ISSET(fd_array[i], &readfds)){ //连接事件就绪
				//获取连接
				struct sockaddr_in peer;
				memset(&peer, 0, sizeof(peer));
				socklen_t len = sizeof(peer);
				int sock = accept(_listen_sock, (struct sockaddr*)&peer, &len);
				if (sock < 0){ //获取连接失败
					std::cerr << "accept error" << std::endl;
					continue;
				}
				std::string peer_ip = inet_ntoa(peer.sin_addr);
				int peer_port = ntohs(peer.sin_port);
				std::cout << "get a new link[" << peer_ip << ":" << peer_port << "]" << std::endl;

				if (!SetFdArray(fd_array, num, sock)){ //将获取到的套接字添加到fd_array当中
					close(sock);
					std::cout << "select server is full, close fd: " << sock << std::endl;
				}
			}
			else if (FD_ISSET(fd_array[i], &readfds)){ //读事件就绪
				char buffer[1024];
				ssize_t size = read(fd_array[i], buffer, sizeof(buffer)-1);
				if (size > 0){ //读取成功
					buffer[size] = '\0';
					std::cout << "echo# " << buffer << std::endl;
				}
				else if (size == 0){ //对端连接关闭
					std::cout << "client quit" << std::endl;
					close(fd_array[i]);
					fd_array[i] = DFL_FD; //将该文件描述符从fd_array中清除
				}
				else{
					std::cerr << "read error" << std::endl;
					close(fd_array[i]);
					fd_array[i] = DFL_FD; //将该文件描述符从fd_array中清除
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
			if (fd_array[i] == DFL_FD){ //该位置没有被使用
				fd_array[i] = fd;
				return true;
			}
		}
		return false; //fd_array数组已满
	}
};


struct timeval timeout = { 0, 0 }; //每隔0秒timeout一次
switch (select(maxfd + 1, &readfds, nullptr, nullptr, &timeout)){
	case 0:
		std::cout << "timeout..." << std::endl;
		break;
	case -1:
		std::cerr << "select error" << std::endl;                                   
		break;
	default:
		//正常的事件处理                                                          
		std::cout << "有事件发生..." << std::endl;
		//HandlerEvent(readfds, fd_array, NUM);                                   
		break;
}

struct timeval timeout = { 5, 0 }; //每隔5秒timeout一次
switch (select(maxfd + 1, &readfds, nullptr, nullptr, &timeout)){
case 0:
	std::cout << "timeout: " << timeout.tv_sec << std::endl;
	break;
case -1:
	std::cerr << "select error" << std::endl;
	break;
default:
	//正常的事件处理                                                          
	std::cout << "有事件发生... timeout: " << timeout.tv_sec << std::endl;
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
