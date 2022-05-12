//对特定内存区域进行清空
void bzero(void *s, size_t n);

//监听
int listen(int sockfd, int backlog);

//获取连接
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

//连接服务器
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
		//创建套接字
		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_listen_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//绑定
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = INADDR_ANY;
		
		if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
		//监听
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
			if (size > 0){ //读取成功
				buffer[size] = '\0';
				std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;

				write(sock, buffer, size);
			}
			else if (size == 0){ //对端关闭连接
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //读取失败
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //归还文件描述符
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
	void Start()
	{
		for (;;){
			//获取连接
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

			//处理请求
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
	int _listen_sock; //监听套接字
	int _port; //端口号
};



#include "tcp_server.hpp"

void Usage(std::string proc)
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
	TcpServer* svr = new TcpServer(port);
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

class TcpClient
{
public:
	TcpClient(std::string server_ip, int server_port)
		: _sock(-1)
		, _server_ip(server_ip)
		, _server_port(server_port)
	{}
	void InitClient()
	{
		//创建套接字
		_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
	}
	void Request()
	{
		std::string msg;
		char buffer[1024];
		while (true){
			std::cout << "Please Enter# ";
			getline(std::cin, msg);

			write(_sock, msg.c_str(), msg.size());

			ssize_t size = read(_sock, buffer, sizeof(buffer)-1);
			if (size > 0){
				buffer[size] = '\0';
				std::cout << "server echo# " << buffer << std::endl;
			}
			else if (size == 0){
				std::cout << "server close!" << std::endl;
				break;
			}
			else{
				std::cerr << "read error!" << std::endl;
				break;
			}
		}
	}
	void Start()
	{
		struct sockaddr_in peer;
		memset(&peer, '\0', sizeof(peer));
		peer.sin_family = AF_INET;
		peer.sin_port = htons(_server_port);
		peer.sin_addr.s_addr = inet_addr(_server_ip.c_str());
		
		if (connect(_sock, (struct sockaddr*)&peer, sizeof(peer)) == 0){ //connect success
			std::cout << "connect success..." << std::endl;
			Request(); //发起请求
		}
		else{ //connect error
			std::cerr << "connect failed..." << std::endl;
			exit(3);
		}
	}
	~TcpClient()
	{
		if (_sock >= 0){
			close(_sock);
		}
	}
private:
	int _sock; //套接字
	std::string _server_ip; //服务端IP地址
	int _server_port; //服务端端口号
};

#include "tcp_client.hpp"

void Usage(std::string proc)
{
	std::cout << "Usage: " << proc << "server_ip server_port" << std::endl;
}
int main(int argc, char* argv[])
{
	if (argc != 3){
		Usage(argv[0]);
		exit(1);
	}
	std::string server_ip = argv[1];
	int server_port = atoi(argv[2]);
	TcpClient* clt = new TcpClient(server_ip, server_port);
	clt->InitClient();
	clt->Start();
	return 0;
}

//忽略SIGCHLD信号
//tcp_server.hpp
#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
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
		//创建套接字
		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_listen_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//绑定
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = INADDR_ANY;
		
		if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
		//监听
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
			if (size > 0){ //读取成功
				buffer[size] = '\0';
				std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

				write(sock, buffer, size);
			}
			else if (size == 0){ //对端关闭连接
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //读取失败
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //归还文件描述符
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
	void Start()
	{
		signal(SIGCHLD, SIG_IGN); //忽略SIGCHLD信号
		for (;;){
			//获取连接
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
			std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
			
			pid_t id = fork();
			if (id == 0){ //child
				//处理请求
				Service(sock, client_ip, client_port);
				exit(0); //子进程提供完服务退出
			}
		}
	}
	~TcpServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	int _listen_sock; //监听套接字
	int _port; //端口号
};



//孙子进程提供服务
#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
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
		//创建套接字
		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_listen_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//绑定
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = INADDR_ANY;

		if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
		//监听
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
			if (size > 0){ //读取成功
				buffer[size] = '\0';
				std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

				write(sock, buffer, size);
			}
			else if (size == 0){ //对端关闭连接
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //读取失败
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //归还文件描述符
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
	void Start()
	{
		for (;;){
			//获取连接
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
			std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
			
			pid_t id = fork();
			if (id == 0){ //child
				close(_listen_sock); //child关闭监听套接字
				if (fork() > 0){
					exit(0); //爸爸进程直接退出
				}
				//处理请求
				Service(sock, client_ip, client_port); //孙子进程提供服务
				exit(0); //孙子进程提供完服务退出
			}
			close(sock); //father关闭为连接提供服务的套接字
			waitpid(id, nullptr, 0); //等待爸爸进程
		}
	}
	~TcpServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	int _listen_sock; //监听套接字
	int _port; //端口号
};


//多线程
#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define BACKLOG 5

class Param
{
public:
	Param(int sock, std::string ip, int port)
		: _sock(sock)
		, _ip(ip)
		, _port(port)
	{}
	~Param()
	{}
public:
	int _sock;
	std::string _ip;
	int _port;
};

class TcpServer
{
public:
	TcpServer(int port)
		: _listen_sock(-1)
		, _port(port)
	{}
	void InitServer()
	{
		//创建套接字
		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_listen_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//绑定
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = INADDR_ANY;
		
		if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
		//监听
		if (listen(_listen_sock, BACKLOG) < 0){
			std::cerr << "listen error" << std::endl;
			exit(4);
		}
	}
	static void Service(int sock, std::string client_ip, int client_port)
	{
		char buffer[1024];
		while (true){
			ssize_t size = read(sock, buffer, sizeof(buffer)-1);
			if (size > 0){ //读取成功
				buffer[size] = '\0';
				std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

				write(sock, buffer, size);
			}
			else if (size == 0){ //对端关闭连接
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //读取失败
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //归还文件描述符
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
	static void* HandlerRequest(void* arg)
	{
		pthread_detach(pthread_self()); //分离线程
		//int sock = *(int*)arg;
		Param* p = (Param*)arg;

		Service(p->_sock, p->_ip, p->_port); //线程为客户端提供服务

		delete p; //释放参数占用的堆空间
		return nullptr;
	}
	void Start()
	{
		for (;;){
			//获取连接
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
			std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;

			//int* p = new int(sock);
			Param* p = new Param(sock, client_ip, client_port);
			pthread_t tid;
			pthread_create(&tid, nullptr, HandlerRequest, p);
		}
	}
	~TcpServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	int _listen_sock; //监听套接字
	int _port; //端口号
};



//线程池
//ThreadPool.hpp
#pragma once

#include <iostream>
#include <unistd.h>
#include <queue>
#include <pthread.h>

#define NUM 5

//线程池
template<class T>
class ThreadPool
{
private:
	bool IsEmpty()
	{
		return _task_queue.size() == 0;
	}
	void LockQueue()
	{
		pthread_mutex_lock(&_mutex);
	}
	void UnLockQueue()
	{
	    pthread_mutex_unlock(&_mutex);
	}
	void Wait()
	{
	    pthread_cond_wait(&_cond, &_mutex);
	}
	void WakeUp()
	{
	    pthread_cond_signal(&_cond);
	}
public:
	ThreadPool(int num = NUM)
		: _thread_num(num)
	{
		pthread_mutex_init(&_mutex, nullptr);
		pthread_cond_init(&_cond, nullptr);
	}
	~ThreadPool()
	{
	    pthread_mutex_destroy(&_mutex);
	    pthread_cond_destroy(&_cond);
	}
	//线程池中线程的执行例程
	static void* Routine(void* arg)
	{
	    pthread_detach(pthread_self());
	    ThreadPool* self = (ThreadPool*)arg;
	    //不断从任务队列获取任务进行处理
		while (true){
			self->LockQueue();
			while (self->IsEmpty()){
				self->Wait();
			}
			T task;
			self->Pop(task);
			self->UnLockQueue();
			
			task.Run(); //处理任务
		}
	}
	void ThreadPoolInit()
	{
		pthread_t tid;
		for (int i = 0; i < _thread_num; i++){
			pthread_create(&tid, nullptr, Routine, this); //注意参数传入this指针
		}
	}
	//往任务队列塞任务（主线程调用）
	void Push(const T& task)
	{
	    LockQueue();
	    _task_queue.push(task);
	    UnLockQueue();
	    WakeUp();
	}
	//从任务队列获取任务（线程池中的线程调用）
	void Pop(T& task)
	{
	    task = _task_queue.front();
	    _task_queue.pop();
	}
	
private:
	std::queue<T> _task_queue; //任务队列
	int _thread_num; //线程池中线程的数量
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

//任务类
#pragma once

#include <iostream>
#include <string>
#include <unistd.h>

class Handler
{
public:
	Handler()
	{}
	~Handler()
	{}
	void operator()(int sock, std::string client_ip, int client_port)
	{
		char buffer[1024];
		while (true){
			ssize_t size = read(sock, buffer, sizeof(buffer)-1);
			if (size > 0){ //读取成功
				buffer[size] = '\0';
				std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

				write(sock, buffer, size);
			}
			else if (size == 0){ //对端关闭连接
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //读取失败
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //归还文件描述符
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
};

class Task
{
public:
	Task()
	{}
	Task(int sock, std::string client_ip, int client_port)
		: _sock(sock)
		, _client_ip(client_ip)
		, _client_port(client_port)
	{}
	~Task()
	{}
	//任务处理函数
	void Run()
	{
	    _handler(_sock, _client_ip, _client_port); //调用仿函数
	}
private:
	int _sock; //套接字
	std::string _client_ip; //IP地址
	int _client_port; //端口号
	Handler _handler; //处理方法
};


//tcp_server
#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "ThreadPool.hpp"
#include "Task.hpp"

#define BACKLOG 5

class TcpServer
{
public:
	TcpServer(int port)
		: _listen_sock(-1)
		, _port(port)
		, _tp(nullptr)
	{}
	void InitServer()
	{
		//创建套接字
		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_listen_sock < 0){
			std::cerr << "socket error" << std::endl;
			exit(2);
		}
		//绑定
		struct sockaddr_in local;
		memset(&local, '\0', sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(_port);
		local.sin_addr.s_addr = INADDR_ANY;

		if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
			std::cerr << "bind error" << std::endl;
			exit(3);
		}
		//监听
		if (listen(_listen_sock, BACKLOG) < 0){
			std::cerr << "listen error" << std::endl;
			exit(4);
		}

		_tp = new ThreadPool<Task>(); //构造线程池对象
	}
	void Start()
	{
		_tp->ThreadPoolInit(); //初始化线程池
		for (;;){
			//获取连接
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
			std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
			
			Task task(sock, client_ip, client_port); //构造任务
			_tp->Push(task); //将任务Push进任务队列
		}
	}
	~TcpServer()
	{
		if (_listen_sock >= 0){
			close(_listen_sock);
		}
	}
private:
	int _listen_sock; //监听套接字
	int _port; //端口号
	ThreadPool<Task>* _tp; //线程池
};

//英译汉TCP服务器
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unistd.h>

class Handler
{
public:
	Handler()
	{}
	~Handler()
	{}
	void operator()(int sock, std::string client_ip, int client_port)
	{
		//only for test
		std::unordered_map<std::string, std::string> dict;
		dict.insert(std::make_pair("dragon", "龙"));
		dict.insert(std::make_pair("blog", "博客"));
		dict.insert(std::make_pair("socket", "套接字"));
		
		char buffer[1024];
		std::string value;
		while (true){
			ssize_t size = read(sock, buffer, sizeof(buffer)-1);
			if (size > 0){ //读取成功
				buffer[size] = '\0';
				std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

				std::string key = buffer;
				auto it = dict.find(key);
				if (it != dict.end()){
					value = it->second;
				}
				else{
					value = key;
				}
				write(sock, value.c_str(), value.size());
			}
			else if (size == 0){ //对端关闭连接
				std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
			}
			else{ //读取失败
				std::cerr << sock << " read error!" << std::endl;
				break;
			}
		}
		close(sock); //归还文件描述符
		std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
	}
};



//字符串IP转整数IP
int inet_aton(const char *cp, struct in_addr *inp);

in_addr_t inet_addr(const char *cp);

int inet_pton(int af, const char *src, void *dst);

//整数IP转字符串IP
char *inet_ntoa(struct in_addr in);

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

void* Func1(void* arg)
{
	struct sockaddr_in* p = (struct sockaddr_in*)arg;
	while (1){
		char* ptr1 = inet_ntoa(p->sin_addr);
		std::cout << "ptr1: " << ptr1 << std::endl;
		sleep(1);
	}
}
void* Func2(void* arg)
{
	struct sockaddr_in* p = (struct sockaddr_in*)arg;
	while (1){
		char* ptr2 = inet_ntoa(p->sin_addr);
		std::cout << "ptr2: " << ptr2 << std::endl;
		sleep(1);
	}
}
int main()
{
	struct sockaddr_in addr1;
	struct sockaddr_in addr2;
	addr1.sin_addr.s_addr = 0;
	addr2.sin_addr.s_addr = 0xffffffff;
	
	pthread_t tid1 = 0;
	pthread_create(&tid1, nullptr, Func1, &addr1);
	pthread_t tid2 = 0;
	pthread_create(&tid2, nullptr, Func2, &addr2);
	
	pthread_join(tid1, nullptr);
	pthread_join(tid2, nullptr);
	return 0;
}
