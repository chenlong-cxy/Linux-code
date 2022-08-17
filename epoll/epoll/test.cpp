int epoll_create(int size);

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);



#pragma once

#include "socket.hpp"
#include <sys/epoll.h>

#define BACK_LOG 5
#define SIZE 256
#define MAX_NUM 64

class EpollServer{
private:
	int _listen_sock; //�����׽���
	int _port; //�˿ں�
	int _epfd; //epollģ��
public:
	EpollServer(int port)
		: _port(port)
	{}
	void InitEpollServer()
	{
		_listen_sock = Socket::SocketCreate();
		Socket::SocketBind(_listen_sock, _port);
		Socket::SocketListen(_listen_sock, BACK_LOG);
		
		//����epollģ��
		_epfd = epoll_create(SIZE);
		if (_epfd < 0){
			std::cerr << "epoll_create error" << std::endl;
			exit(5);
		}
	}
	void Run()
	{
		AddEvent(_listen_sock, EPOLLIN); //�������׽�����ӵ�epollģ���У�����������¼�
		for (;;){
			struct epoll_event revs[MAX_NUM];
			int num = epoll_wait(_epfd, revs, MAX_NUM, -1);
			if (num < 0){
				std::cerr << "epoll_wait error" << std::endl;
				continue;
			}
			else if (num == 0){
				std::cout << "timeout..." << std::endl;
				continue;
			}
			else{
				//�������¼�����
				//std::cout<<"���¼�����..."<<std::endl;
				HandlerEvent(revs, num);
			}
		}
	}
	void HandlerEvent(struct epoll_event revs[], int num)
	{
		for (int i = 0; i < num; i++){
			int fd = revs[i].data.fd; //�������ļ�������
			if (fd == _listen_sock&&revs[i].events&EPOLLIN){ //�����¼�����
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
				
				AddEvent(sock, EPOLLIN); //����ȡ�����׽�����ӵ�epollģ���У�����������¼�
			}
			else if (revs[i].events&EPOLLIN){ //���¼�����
				char buffer[64];
				ssize_t size = recv(fd, buffer, sizeof(buffer)-1, 0);
				if (size > 0){ //��ȡ�ɹ�
					buffer[size] = '\0';
					std::cout << "echo# " << buffer << std::endl;
				}
				else if (size == 0){ //�Զ����ӹر�
					std::cout << "client quit" << std::endl;
					close(fd);
					DelEvent(fd); //���ļ���������epollģ����ɾ��
				}
				else{
					std::cerr << "recv error" << std::endl;
					close(fd);
					DelEvent(fd); //���ļ���������epollģ����ɾ��
				}
			}
		}
	}
	~EpollServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
		if (_epfd >= 0){
			close(_epfd);
		}
	}
private:
	void AddEvent(int sock, uint32_t event)
	{
		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = sock;
		
		epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev);
	}
	void DelEvent(int sock)
	{
		epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
	}
};


#include "epoll_server.hpp"
#include <string>

static void Usage(std::string proc)
{
	std::cout << "Usage: " << proc << " port" << std::endl;
}
int main(int argc, char* argv[])
{
	if (argc != 2){
		Usage(argv[0]);
		exit(1);
	}
	int port = atoi(argv[1]);

	EpollServer* svr = new EpollServer(port);
	svr->InitEpollServer();
	svr->Run();
	
	return 0;
}

