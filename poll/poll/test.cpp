int poll(struct pollfd *fds, nfds_t nfds, int timeout);


#pragma once

#include "socket.hpp"
#include <poll.h>

#define BACK_LOG 5
#define NUM 1024
#define DFL_FD - 1

class PollServer{
private:
	int _listen_sock; //监听套接字
	int _port; //端口号
public:
	PollServer(int port)
		: _port(port)
	{}
	void InitPollServer()
	{
		_listen_sock = Socket::SocketCreate();
		Socket::SocketBind(_listen_sock, _port);
		Socket::SocketListen(_listen_sock, BACK_LOG);
	}
	void Run()
	{
		struct pollfd fds[NUM];
		ClearPollfds(fds, NUM, DFL_FD); //清空数组中的所有位置
		SetPollfds(fds, NUM, _listen_sock); //将监听套接字添加到数组中，并关心其读事件
		for (;;){
			switch (poll(fds, NUM, -1)){
			case 0:
				std::cout << "timeout..." << std::endl;
				break;
			case -1:
				std::cerr << "poll error" << std::endl;
				break;
			default:
				//正常的事件处理
				//std::cout<<"有事件发生..."<<std::endl;
				HandlerEvent(fds, NUM);
				break;
			}
		}
	}
	void HandlerEvent(struct pollfd fds[], int num)
	{
		for (int i = 0; i < num; i++){
			if (fds[i].fd == DFL_FD){ //跳过无效的位置
				continue;
			}
			if (fds[i].fd == _listen_sock&&fds[i].revents&POLLIN){ //连接事件就绪
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
				
				if (!SetPollfds(fds, NUM, sock)){ //将获取到的套接字添加到fds数组中，并关心其读事件
					close(sock);
					std::cout << "poll server is full, close fd: " << sock << std::endl;
				}
			}
			else if (fds[i].revents&POLLIN){ //读事件就绪
				char buffer[1024];
				ssize_t size = read(fds[i].fd, buffer, sizeof(buffer)-1);
				if (size > 0){ //读取成功
					buffer[size] = '\0';
					std::cout << "echo# " << buffer << std::endl;
				}
				else if (size == 0){ //对端连接关闭
					std::cout << "client quit" << std::endl;
					close(fds[i].fd);
					UnSetPollfds(fds, i); //将该文件描述符从fds数组中清除
				}
				else{
					std::cerr << "read error" << std::endl;
					close(fds[i].fd);
					UnSetPollfds(fds, i); //将该文件描述符从fds数组中清除
				}
			}
		}
	}
	~PollServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	void ClearPollfds(struct pollfd fds[], int num, int default_fd)
	{
		for (int i = 0; i < num; i++){
			fds[i].fd = default_fd;
			fds[i].events = 0;
			fds[i].revents = 0;
		}
	}
	bool SetPollfds(struct pollfd fds[], int num, int fd)
	{
		for (int i = 0; i < num; i++){
			if (fds[i].fd == DFL_FD){ //该位置没有被使用
				fds[i].fd = fd;
				fds[i].events |= POLLIN; //添加读事件到events当中
				return true;
			}
		}
		return false; //fds数组已满
	}
	void UnSetPollfds(struct pollfd fds[], int pos)
	{
		fds[pos].fd = DFL_FD;
		fds[pos].events = 0;
		fds[pos].revents = 0;
	}
};


#include "poll_server.hpp"
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
	PollServer* svr = new PollServer(port);
	svr->InitPollServer();
	svr->Run();
	
	return 0;
}
