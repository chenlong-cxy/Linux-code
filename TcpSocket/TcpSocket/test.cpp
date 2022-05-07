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
				std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;

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
		//�����׽���
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
			Request(); //��������
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
	int _sock; //�׽���
	std::string _server_ip; //�����IP��ַ
	int _server_port; //����˶˿ں�
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

//����SIGCHLD�ź�
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
		signal(SIGCHLD, SIG_IGN); //����SIGCHLD�ź�
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
			std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
			
			pid_t id = fork();
			if (id == 0){ //child
				//��������
				Service(sock, client_ip, client_port);
				exit(0); //�ӽ����ṩ������˳�
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
	int _listen_sock; //�����׽���
	int _port; //�˿ں�
};



//���ӽ����ṩ����
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
			std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
			
			pid_t id = fork();
			if (id == 0){ //child
				close(_listen_sock); //child�رռ����׽���
				if (fork() > 0){
					exit(0); //�ְֽ���ֱ���˳�
				}
				//��������
				Service(sock, client_ip, client_port); //���ӽ����ṩ����
				exit(0); //���ӽ����ṩ������˳�
			}
			close(sock); //father�ر�Ϊ�����ṩ������׽���
			waitpid(id, nullptr, 0); //�ȴ��ְֽ���
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


//���߳�
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
					19         Param(int sock, std::string ip, int port)
						20 : _sock(sock)
						21, _ip(ip)
						22, _port(port)
						23         {}
					24         ~Param()
						25         {}
					26     public:
						27         int _sock;
						28         std::string _ip;
						29         int _port;
						30 };
			31
				32 class TcpServer
				33 {
				34     public:
					35         TcpServer(int port)
						36 : _listen_sock(-1)
						37, _port(port)
						38         {}
					39         void InitServer()
						40         {
						41             //�����׽���
							42             _listen_sock = socket(AF_INET, SOCK_STREAM, 0);
						43             if (_listen_sock < 0){
							44                 std::cerr << "socket error" << std::endl;
							45                 exit(2);
							46
						}
						47             //��
							48             struct sockaddr_in local;
						49             memset(&local, '\0', sizeof(local));
						50             local.sin_family = AF_INET;
						51             local.sin_port = htons(_port);
						52             local.sin_addr.s_addr = INADDR_ANY;
						53
							54             if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
							55                 std::cerr << "bind error" << std::endl;
							56                 exit(3);
							57
						}
						58             //����
							59             if (listen(_listen_sock, BACKLOG) < 0){
							60                 std::cerr << "listen error" << std::endl;
							61                 exit(4);
							62
						}
						63         }
					64         static void Service(int sock, std::string client_ip, int client_port)
						65         {
						66             char buffer[1024];
						67             while (true){
							68                 ssize_t size = read(sock, buffer, sizeof(buffer)-1);
							69                 if (size > 0){ //��ȡ�ɹ�
								70                     buffer[size] = '\0';
								71                     std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;
								72
									73                     write(sock, buffer, size);
								74
							}
							75                 else if (size == 0){ //�Զ˹ر�����
								76                     std::cout << client_ip << ":" << client_port << " close!" << std::endl;
								77                     break;
								78
							}
							79                 else{ //��ȡʧ��
								80                     std::cerr << sock << " read error!" << std::endl;
								81                     break;
								82
							}
							83
						}
						84             close(sock); //�黹�ļ�������
						85             std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
						86         }
					87         static void* HandlerRequest(void* arg)
						88         {
						89             pthread_detach(pthread_self()); //�����߳�
						90             //int sock = *(int*)arg;
							91             Param* p = (Param*)arg;
						92
							93             Service(p->_sock, p->_ip, p->_port); //�߳�Ϊ�ͻ����ṩ����
						94
							95             delete p; //�ͷŲ���ռ�õĶѿռ�
						96             return nullptr;
						97         }
					98         void Start()
						99         {
						100             for (;;){
							101                 //��ȡ����
								102                 struct sockaddr_in peer;
							103                 memset(&peer, '\0', sizeof(peer));
							104                 socklen_t len = sizeof(peer);
							105                 int sock = accept(_listen_sock, (struct sockaddr*)&peer, &len);
							106                 if (sock < 0){
								107                     std::cerr << "accept error, continue next" << std::endl;
								108                     continue;
								109
							}
							110                 std::string client_ip = inet_ntoa(peer.sin_addr);
							111                 int client_port = ntohs(peer.sin_port);
							112                 std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
							113
								114                 //int* p = new int(sock);
								115                 Param* p = new Param(sock, client_ip, client_port);
							116                 pthread_t tid;
							117                 pthread_create(&tid, nullptr, HandlerRequest, p);
							118
						}
						119         }
					120         ~TcpServer()
						121         {
						122             if (_listen_sock >= 0){
							123                 close(_listen_sock);
							124
						}
						125         }
					126     private:
						127         int _listen_sock; //�����׽���
						128         int _port; //�˿ں�
						129 };
			~



//�̳߳�
//ThreadPool.hpp
			1 #pragma once
				2
				3 #include <iostream>
				4 #include <unistd.h>
				5 #include <queue>
				6 #include <pthread.h>
				7
				8 #define NUM 5
				9
				10 //�̳߳�
				11 template<class T>
				12 class ThreadPool
				13 {
				14     private:
					15         bool IsEmpty()
						16         {
						17             return _task_queue.size() == 0;
						18         }
					19         void LockQueue()
						20         {
						21             pthread_mutex_lock(&_mutex);
						22         }
					23         void UnLockQueue()
						24         {
						25             pthread_mutex_unlock(&_mutex);
						26         }
					27         void Wait()
						28         {
						29             pthread_cond_wait(&_cond, &_mutex);
						30         }
					31         void WakeUp()
						32         {
						33             pthread_cond_signal(&_cond);
						34         }
					35     public:
						36         ThreadPool(int num = NUM)
							37 : _thread_num(num)
							38         {
							39             pthread_mutex_init(&_mutex, nullptr);
							40             pthread_cond_init(&_cond, nullptr);
							41         }
						42         ~ThreadPool()
							43         {
							44             pthread_mutex_destroy(&_mutex);
							45             pthread_cond_destroy(&_cond);
							46         }
						47         //�̳߳����̵߳�ִ������
							48         static void* Routine(void* arg)
							49         {
							50             pthread_detach(pthread_self());
							51             ThreadPool* self = (ThreadPool*)arg;
							52             //���ϴ�������л�ȡ������д���
								53             while (true){
								54                 //sleep(1);
									55                 self->LockQueue();
								56                 while (self->IsEmpty()){
									57                     self->Wait();
									58
								}
								59                 T task;
								60                 self->Pop(task);
								61                 self->UnLockQueue();
								62
									63                 task.Run(); //��������
								64
							}
							65         }
						66         void ThreadPoolInit()
							67         {
							68             pthread_t tid;
							69             for (int i = 0; i < _thread_num; i++){
								70                 pthread_create(&tid, nullptr, Routine, this); //ע���������thisָ��
								71
							}
							72         }
						73         //������������������̵߳��ã�
							74         void Push(const T& task)
							75         {
							76             LockQueue();
							77             _task_queue.push(task);
							78             UnLockQueue();
							79             WakeUp();
							80         }
						81         //��������л�ȡ�����̳߳��е��̵߳��ã�
							82         void Pop(T& task)
							83         {
							84             task = _task_queue.front();
							85             _task_queue.pop();
							86         }
						87
							88     private:
								89         std::queue<T> _task_queue; //�������
								90         int _thread_num; //�̳߳����̵߳�����
								91         pthread_mutex_t _mutex;
								92         pthread_cond_t _cond;
								93 };

//������
			1 #pragma once
				2
				3 #include <iostream>
				4 #include <string>
				5 #include <unistd.h>
				6
				7 class Handler
				8 {
				9     public:
					10         Handler()
						11         {}
					12         ~Handler()
						13         {}
					14         void operator()(int sock, std::string client_ip, int client_port)
						15         {
						16             char buffer[1024];
						17             while (true){
							18                 ssize_t size = read(sock, buffer, sizeof(buffer)-1);
							19                 if (size > 0){ //��ȡ�ɹ�
								20                     buffer[size] = '\0';
								21                     std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;
								22
									23                     write(sock, buffer, size);
								24
							}
							25                 else if (size == 0){ //�Զ˹ر�����
								26                     std::cout << client_ip << ":" << client_port << " close!" << std::endl;
								27                     break;
								28
							}
							29                 else{ //��ȡʧ��
								30                     std::cerr << sock << " read error!" << std::endl;
								31                     break;
								32
							}
							33
						}
						34             close(sock); //�黹�ļ�������
						35             std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
						36         }
					37 };
			38
				39 class Task
				40 {
				41     public:
					42         Task()
						43         {}
					44         Task(int sock, std::string client_ip, int client_port)
						45 : _sock(sock)
						46, _client_ip(client_ip)
						47, _client_port(client_port)
						48         {}
					49         ~Task()
						50         {}
					51         void Run()
						52         {
						53             _handler(_sock, _client_ip, _client_port); //���÷º���
						54         }
					55     private:
						56         int _sock; //�׽���
						57         std::string _client_ip; //IP��ַ
						58         int _client_port; //�˿ں�
						59         Handler _handler; //������
						60 };


//tcp_server
			1 #pragma once
				2
				3 #include <iostream>
				4 #include <cstring>
				5 #include <sys/types.h>
				6 #include <sys/socket.h>
				7 #include <netinet/in.h>
				8 #include <arpa/inet.h>
				9 #include <sys/wait.h>
				10 #include <signal.h>
				11 #include <unistd.h>
				12 #include <pthread.h>
				13 #include "ThreadPool.hpp"
				14 #include "Task.hpp"
				15
				16 #define BACKLOG 5
				17
				18 class TcpServer
				19 {
				20     public:
					21         TcpServer(int port)
						22 : _listen_sock(-1)
						23, _port(port)
						24, _tp(nullptr)
						25         {}
					26         void InitServer()
						27         {
						28             //�����׽���
							29             _listen_sock = socket(AF_INET, SOCK_STREAM, 0);
						30             if (_listen_sock < 0){
							31                 std::cerr << "socket error" << std::endl;
							32                 exit(2);
							33
						}
						34             //��
							35             struct sockaddr_in local;
						36             memset(&local, '\0', sizeof(local));
						37             local.sin_family = AF_INET;
						38             local.sin_port = htons(_port);
						39             local.sin_addr.s_addr = INADDR_ANY;
						40
							41             if (bind(_listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
							42                 std::cerr << "bind error" << std::endl;
							43                 exit(3);
							44
						}
						45             //����
							46             if (listen(_listen_sock, BACKLOG) < 0){
							47                 std::cerr << "listen error" << std::endl;
							48                 exit(4);
							49
						}
						50
							51             _tp = new ThreadPool<Task>(); //�����̳߳ض���
						52         }
					53         void Start()
						54         {
						55             _tp->ThreadPoolInit(); //��ʼ���̳߳�
						56             for (;;){
							57                 //��ȡ����
								58                 struct sockaddr_in peer;
							59                 memset(&peer, '\0', sizeof(peer));
							60                 socklen_t len = sizeof(peer);
							61                 int sock = accept(_listen_sock, (struct sockaddr*)&peer, &len);
							62                 if (sock < 0){
								63                     std::cerr << "accept error, continue next" << std::endl;
								64                     continue;
								65
							}
							66                 std::string client_ip = inet_ntoa(peer.sin_addr);
							67                 int client_port = ntohs(peer.sin_port);
							68                 std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
							69
								70                 Task task(sock, client_ip, client_port); //��������
							71                 _tp->Push(task); //������Push���������
							72
						}
						73         }
					74         ~TcpServer()
						75         {
						76             if (_listen_sock >= 0){
							77                 close(_listen_sock);
							78
						}
						79         }
					80     private:
						81         int _listen_sock; //�����׽���
						82         int _port; //�˿ں�
						83         ThreadPool<Task>* _tp; //�̳߳�
						84 };
