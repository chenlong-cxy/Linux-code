#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.hpp"

using namespace std;

void* Routine(void* arg)
{
	pthread_detach(pthread_self()); //分离线程
	int sock = *(int*)arg;
	delete (int*)arg;
	
	while (true){
		request_t rq;
		ssize_t size = recv(sock, &rq, sizeof(rq), 0);
		if (size > 0){
			response_t rp = { 0, 0 };
			switch (rq.op){
			case '+':
				rp.result = rq.x + rq.y;
				break;
			case '-':
				rp.result = rq.x - rq.y;
				break;
			case '*':
				rp.result = rq.x * rq.y;
				break;
			case '/':
				if (rq.y == 0){
					rp.code = 1; //除0错误
				}
				else{
					rp.result = rq.x / rq.y;
				}
				break;
			case '%':
				if (rq.y == 0){
					rp.code = 2; //模0错误
				}
				else{
					rp.result = rq.x % rq.y;
				}
				break;
			default:
				rp.code = 3; //非法运算
				break;
			}
			send(sock, &rp, sizeof(rp), 0);
		}
		else if (size == 0){
			cout << "service done" << endl;
			break;
		}
		else{
			cerr << "read error" << endl;
			break;
		}
	}
	close(sock);
	return nullptr;
}
int main(int argc, char* argv[])
{
	if (argc != 2){
		cerr << "Usage: " << argv[0] << " port" << endl;
		exit(1);
	}
	int port = atoi(argv[1]);

	//创建套接字
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0){
		cerr << "socket error!" << endl;
		exit(2);
	}

	//绑定
	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		cerr << "bind error!" << endl;
		exit(3);
	}

	//监听
	if (listen(listen_sock, 5) < 0){
		cerr << "listen error!" << endl;
		exit(4);
	}

	//启动服务器
	struct sockaddr peer;
	memset(&peer, 0, sizeof(peer));
	for (;;){
		socklen_t len = sizeof(peer);
		int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if (sock < 0){
			cerr << "accept error!" << endl;
			continue;
		}
		pthread_t tid = 0;
		int* p = new int(sock);
		pthread_create(&tid, nullptr, Routine, p);
	}
	return 0;
}



#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3){
		cerr << "Usage: " << argv[0] << " server_ip server_port" << endl;
		exit(1);
	}
	string server_ip = argv[1];
	int server_port = atoi(argv[2]);

	//创建套接字
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		cerr << "socket error!" << endl;
		exit(2);
	}

	//连接服务器
	struct sockaddr_in peer;
	memset(&peer, 0, sizeof(peer));
	peer.sin_family = AF_INET;
	peer.sin_port = htons(server_port);
	peer.sin_addr.s_addr = inet_addr(server_ip.c_str());
	if (connect(sock, (struct sockaddr*)&peer, sizeof(peer)) < 0){
		cerr << "connect failed!" << endl;
		exit(3);
	}

	//发起请求
	while (true){
		//构建请求
		request_t rq;
		cout << "请输入左操作数# ";
		cin >> rq.x;
		cout << "请输入右操作数# ";
		cin >> rq.y;
		cout << "请输入需要进行的操作[+-*/%]# ";
		cin >> rq.op;
		send(sock, &rq, sizeof(rq), 0);
		
		//接收请求响应
		response_t rp;
		recv(sock, &rp, sizeof(rp), 0);
		cout << "status: " << rp.code << endl;
		cout << rq.x << rq.op << rq.y << "=" << rp.result << endl;
	}
	return 0;
}


#pragma once

//请求
typedef struct request{
	int x;
	int y;
	char op;
}request_t;

//响应
typedef struct response{
	int code;
	int result;
}response_t;
