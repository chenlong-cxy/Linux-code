int poll(struct pollfd *fds, nfds_t nfds, int timeout);


#pragma once

#include "socket.hpp"
#include <poll.h>

#define BACK_LOG 5
#define NUM 1024
#define DFL_FD - 1

class PollServer{
private:
	int _listen_sock; //�����׽���
	int _port; //�˿ں�
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
		ClearPollfds(fds, NUM, DFL_FD); //��������е�����λ��
		SetPollfds(fds, NUM, _listen_sock); //�������׽�����ӵ������У�����������¼�
		for (;;){
			switch (poll(fds, NUM, -1)){
			case 0:
				std::cout << "timeout..." << std::endl;
				break;
			case -1:
				std::cerr << "poll error" << std::endl;
				break;
			default:
				//�������¼�����
				//std::cout<<"���¼�����..."<<std::endl;
				HandlerEvent(fds, NUM);
				break;
			}
		}
	}
	void HandlerEvent(struct pollfd fds[], int num)
	{
		for (int i = 0; i < num; i++){
			if (fds[i].fd == DFL_FD){ //������Ч��λ��
				continue;
			}
			if (fds[i].fd == _listen_sock&&fds[i].revents&POLLIN){ //�����¼�����
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
				
				if (!SetPollfds(fds, NUM, sock)){ //����ȡ�����׽�����ӵ�fds�����У�����������¼�
					close(sock);
					std::cout << "poll server is full, close fd: " << sock << std::endl;
				}
			}
			else if (fds[i].revents&POLLIN){ //���¼�����
				char buffer[1024];
				ssize_t size = read(fds[i].fd, buffer, sizeof(buffer)-1);
				if (size > 0){ //��ȡ�ɹ�
					buffer[size] = '\0';
					std::cout << "echo# " << buffer << std::endl;
				}
				else if (size == 0){ //�Զ����ӹر�
					std::cout << "client quit" << std::endl;
					close(fds[i].fd);
					UnSetPollfds(fds, i); //�����ļ���������fds���������
				}
				else{
					std::cerr << "read error" << std::endl;
					close(fds[i].fd);
					UnSetPollfds(fds, i); //�����ļ���������fds���������
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
			if (fds[i].fd == DFL_FD){ //��λ��û�б�ʹ��
				fds[i].fd = fd;
				fds[i].events |= POLLIN; //��Ӷ��¼���events����
				return true;
			}
		}
		return false; //fds��������
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
