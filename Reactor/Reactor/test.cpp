//reactor.hpp

#pragma once

#include "comm.hpp"
#include <unistd.h>
#include <sys/epoll.h>
#include <string>
#include <unordered_map>

#define SIZE 256
#define MAX_NUM 64

//ǰ������
class Reactor;
class EventItem;

typedef int(*callback_t)(EventItem*);

class EventItem{
public:
	int _sock; //�ļ�������
	Reactor* _R; //��ָָ��

	callback_t _recv_handler; //���ص�
	callback_t _send_handler; //д�ص�
	callback_t _error_handler; //�쳣�ص�

	std::string _inbuffer; //���뻺����
	std::string _outbuffer; //���������
public:
	EventItem()
		: _sock(-1)
		, _R(nullptr)
		, _recv_handler(nullptr)
		, _send_handler(nullptr)
		, _error_handler(nullptr)
	{}
	//����ص�
	void ManageCallbacks(callback_t recv_handler, callback_t send_handler, callback_t error_handler)
	{
		_recv_handler = recv_handler;
		_send_handler = send_handler;
		_error_handler = error_handler;
	}
	~EventItem()
	{}
};
class Reactor{
private:
	int _epfd; //epollģ��
	std::unordered_map<int, EventItem> _event_items; //����sock��EventItem�ṹ��ӳ��
public:
	Reactor()
		: _epfd(-1)
	{}
	void InitReactor()
	{
		//����epollģ��
		_epfd = epoll_create(SIZE);
		if (_epfd < 0){
			std::cerr << "epoll_create error" << std::endl;
			exit(5);
		}
	}
	//�¼�������
	void Dispatcher(int timeout)
	{
		struct epoll_event revs[MAX_NUM];
		int num = epoll_wait(_epfd, revs, MAX_NUM, timeout);
		for (int i = 0; i < num; i++){
			int sock = revs[i].data.fd; //�������ļ�������
			if ((revs[i].events&EPOLLERR) || (revs[i].events&EPOLLHUP)){ //�쳣�¼����������ȴ���
				if (_event_items[sock]._error_handler)
					_event_items[sock]._error_handler(&_event_items[sock]); //�����쳣�ص�
			}
			if (revs[i].events&EPOLLIN){ //���¼�����
				if (_event_items[sock]._recv_handler)
					_event_items[sock]._recv_handler(&_event_items[sock]); //���ö��ص�
			}
			if (revs[i].events&EPOLLOUT){ //д�¼�����
				if (_event_items[sock]._send_handler)
					_event_items[sock]._send_handler(&_event_items[sock]); //����д�ص�
			}
		}
	}
	void AddEvent(int sock, uint32_t event, const EventItem& item)
	{
		struct epoll_event ev;
		ev.data.fd = sock;
		ev.events = event;
		
		if (epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev) < 0){ //�����ļ���������ӵ�epollģ�͵���
			std::cerr << "epoll_ctl add error, fd: " << sock << std::endl;
		}
		else{
			//����sock��EventItem�ṹ��ӳ���ϵ
			_event_items.insert({ sock, item });
			std::cout << "���: " << sock << " ��epollģ���У��ɹ�" << std::endl;
		}
	}
	void DelEvent(int sock)
	{
		if (epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr) < 0){ //�����ļ���������epollģ����ɾ��
			std::cerr << "epoll_ctl del error, fd: " << sock << std::endl;
		}
		else{
			//ȡ��sock��EventItem�ṹ��ӳ���ϵ
			_event_items.erase(sock);
			std::cout << "��epollģ����ɾ��: " << sock << "���ɹ�" << std::endl;
		}
	}
	void EnableReadWrite(int sock, bool read, bool write){
		struct epoll_event ev;
		ev.data.fd = sock;
		ev.events = (read ? EPOLLIN : 0) | (write ? EPOLLOUT : 0) | EPOLLET;
		if (epoll_ctl(_epfd, EPOLL_CTL_MOD, sock, &ev) < 0){ //�޸ĸ��ļ�����������Ҫ���ӵ��¼�
			std::cerr << "epoll_ctl mod error, fd: " << sock << std::endl;
		}
	}
	~Reactor()
	{
		if (_epfd >= 0){
			close(_epfd);
		}
	}
};

//app_interface.hpp

#pragma once

#include "comm.hpp"
#include "reactor.hpp"
#include "socket.hpp"
#include "util.hpp"
#include <vector>

//ǰ������
int recver(EventItem* item);
int sender(EventItem* item);
int errorer(EventItem* item);

int accepter(EventItem* item)
{
	while (true){
		struct sockaddr_in peer;
		memset(&peer, 0, sizeof(peer));
		socklen_t len = sizeof(peer);
		int sock = accept(item->_sock, (struct sockaddr*)&peer, &len);
		if (sock < 0){
			if (errno == EAGAIN || errno == EWOULDBLOCK){ //��û�ж�ȡ����ֻ�ǵײ�û��������
				return 0;
			}
			else if (errno == EINTR){ //��ȡ�Ĺ��̱��ź��ж���
				continue;
			}
			else{ //��ȡ����ʧ��
				std::cerr << "accept error" << std::endl;
				return -1;
			}
		}
		SetNonBlock(sock); //�����׽�������Ϊ������
		//����EventItem�ṹ
		EventItem sock_item;
		sock_item._sock = sock;
		sock_item._R = item->_R;
		sock_item.ManageCallbacks(recver, sender, errorer); //ע��ص�����
		
		Reactor* R = item->_R;
		R->AddEvent(sock, EPOLLIN | EPOLLET, sock_item); //�����׽��ּ����Ӧ���¼�ע�ᵽDispatcher��
	}
	return 0;
}
int recver_helper(int sock, std::string* out)
{
	while (true){
		char buffer[128];
		ssize_t size = recv(sock, buffer, sizeof(buffer)-1, 0);
		if (size < 0){
			if (errno == EAGAIN || errno == EWOULDBLOCK){ //���ݶ�ȡ���
				return 0;
			}
			else if (errno == EINTR){ //���ź��жϣ��������Զ�ȡ
				continue;
			}
			else{ //��ȡ����
				return -1;
			}
		}
		else if (size == 0){ //�Զ����ӹر�
			return -1;
		}
		//��ȡ�ɹ�
		buffer[size] = '\0';
		*out += buffer; //����ȡ����������ӵ����׽��ֶ�ӦEventItem�ṹ��inbuffer��
	}
}
int recver(EventItem* item)
{
	if (item->_sock < 0) //���ļ��������Ѿ����ر�
		return -1;
	//1�����ݶ�ȡ
	if (recver_helper(item->_sock, &(item->_inbuffer)) < 0){ //��ȡʧ��
		item->_error_handler(item);
		return -1;
	}

	//2�������и�
	std::vector<std::string> datagrams;
	StringUtil::Split(item->_inbuffer, &datagrams, "X");

	//3�������л�
	for (auto s : datagrams){
		struct data d;
		StringUtil::Deserialize(s, &d._x, &d._y, &d._op);

		//4��ҵ����
		int result = 0;
		switch (d._op)
		{
			case '+':
				result = d._x + d._y;
				break;
			case '-':
				result = d._x - d._y;
				break;
			case '*':
				result = d._x * d._y;
				break;
			case '/':
				if (d._y == 0){
					std::cerr << "Error: div zero!" << std::endl;
					continue; //����������һ������
				}
				else{
					result = d._x / d._y;
				}
				break;
			case '%':
				if (d._y == 0){
					std::cerr << "Error: mod zero!" << std::endl;
					continue; //����������һ������
				}
				else{
					result = d._x % d._y;
				}
				break;
			default:
				std::cerr << "operation error!" << std::endl;
				continue; //����������һ������
		}

		//5���γ���Ӧ����
		std::string response;
		response += std::to_string(d._x);
		response += d._op;
		response += std::to_string(d._y);
		response += "=";
		response += std::to_string(result);
		response += "X"; //�����뱨��֮��ķָ���
		
		//6������Ӧ������ӵ�outbuffer��
		item->_outbuffer += response;
		if (!item->_outbuffer.empty())
			item->_R->EnableReadWrite(item->_sock, true, true); //��д�¼�
	}
	return 0;
}
int sender_helper(int sock, std::string& in)
{
	size_t total = 0; //�ۼ��Ѿ����͵��ֽ���
	while (true){
		ssize_t size = send(sock, in.c_str() + total, in.size() - total, 0);
		if (size < 0){
			if (errno == EAGAIN || errno == EWOULDBLOCK){ //�ײ㷢�ͻ������Ѿ�û�пռ���
				in.erase(0, total); //���Ѿ����͵������Ƴ�outbuffer
				return 1; //������д����ûд��
			}
			else if (errno == EINTR){ //���ź��жϣ���������д��
				continue;
			}
			else{ //д�����
				return -1;
			}
		}
		total += size;
		if (total >= in.size()){
			in.clear(); //���outbuffer
			return 0; //ȫ��д�����
		}
	}
}
int sender(EventItem* item)
{
	if (item->_sock < 0) //���ļ��������Ѿ����ر�
		return -1;

	int ret = sender_helper(item->_sock, item->_outbuffer);
	if (ret == 0){ //ȫ�����ͳɹ������ٹ���д�¼�
		item->_R->EnableReadWrite(item->_sock, true, false);
	}
	else if (ret == 1){ //û�з�����ϣ�����Ҫ��������д�¼�
		item->_R->EnableReadWrite(item->_sock, true, true);
	}
	else{ //д�����
		item->_error_handler(item);
	}
	return 0;
}
int errorer(EventItem* item)
{
	item->_R->DelEvent(item->_sock); //�����ļ���������epollģ����ɾ������ȡ�����ļ�����������EventItem�ṹ��ӳ���ϵ
	close(item->_sock); //�رո��ļ�������

	item->_sock = -1; //��ֹ�رպ����ִ�ж�д�ص�
	return 0;
}

//util.hpp

#pragma once

#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

//�����ļ�������Ϊ������
bool SetNonBlock(int sock)
{
	int fl = fcntl(sock, F_GETFL);
	if (fl < 0){
		std::cerr << "fcntl error" << std::endl;
		return false;
	}
	fcntl(sock, F_SETFL, fl | O_NONBLOCK);
	return true;
}

class StringUtil{
public:
	static void Split(std::string& in, std::vector<std::string>* out, std::string sep)
	{
		int start = 0;
		size_t pos = in.find(sep, start);
		while (pos != std::string::npos){
			out->push_back(in.substr(start, pos - start));
			start = pos + sep.size();
			pos = in.find(sep, start);
		}
		in = in.substr(start);
	}
	static void Deserialize(std::string& in, int* x, int* y, char* op)
	{
		size_t pos = 0;
		for (pos = 0; pos < in.size(); pos++){
			if (in[pos] == '+' || in[pos] == '-' || in[pos] == '*' || in[pos] == '/' || in[pos] == '%')
				break;
		}
		if (pos < in.size()){
			std::string left = in.substr(0, pos);
			std::string right = in.substr(pos + 1);

			*x = atoi(left.c_str());
			*y = atoi(right.c_str());
			*op = in[pos];
		}
		else{
			*op = -1;
		}
	}
};

//server.cc

#include "app_interface.hpp"
#include "reactor.hpp"
#include "socket.hpp"
#include "util.hpp"
#include <string>

#define BACK_LOG 5

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

	//�����������׽��ֵĴ������󶨺ͼ���
	int listen_sock = Socket::SocketCreate();
	SetNonBlock(listen_sock); //�������׽�������Ϊ������
	Socket::SocketBind(listen_sock, port);
	Socket::SocketListen(listen_sock, BACK_LOG);
	
	//����Reactor������ʼ��
	Reactor R;
	R.InitReactor();

	//���������׽��ֶ�Ӧ��EventItem�ṹ            
	EventItem item;
	item._sock = listen_sock;
	item._R = &R;
	item.ManageCallbacks(accepter, nullptr, nullptr); //�����׽���ֻ��Ҫ���Ķ��¼�
	
	//�������׽����йܸ�Dispatcher
	R.AddEvent(listen_sock, EPOLLIN | EPOLLET, item);
	
	//ѭ�������¼��ɷ�
	int timeout = 1000;
	while (true){
		R.Dispatcher(timeout);
	}
	return 0;
}

//socket.hpp

#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
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

//comm.hpp

#pragma once

#include <iostream>

struct data{
	int _x;
	int _y;
	char _op;
};

/*---------------------------------------------------------------------------------*/

//ThreadPool.hpp

#pragma once

#include <iostream>
#include <unistd.h>
#include <queue>
#include <pthread.h>

#define NUM 5

 //�̳߳�
template<class T>
class ThreadPool
{
public:
	//�ṩһ��ȫ�ַ��ʵ�
	static ThreadPool* GetInstance()
	{
		return &_sInst;
	}
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
	~ThreadPool()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_cond);
	}
	//�̳߳����̵߳�ִ������
	static void* Routine(void* arg)
	{
		pthread_detach(pthread_self());
		ThreadPool* self = (ThreadPool*)arg;
		//���ϴ�������л�ȡ������д���
		while (true){
			self->LockQueue();
			while (self->IsEmpty()){
				self->Wait();
			}
			T task;
			self->Pop(task);
			self->UnLockQueue();

			task.Run(); //��������
		}
	}
	void ThreadPoolInit()
	{
		pthread_t tid;
		for (int i = 0; i < _thread_num; i++){
			pthread_create(&tid, nullptr, Routine, this); //ע���������thisָ��
		}
	}
	//������������������̵߳��ã�
	void Push(const T& task)
	{
		LockQueue();
		_task_queue.push(task);
		UnLockQueue();
		WakeUp();
	}
	//��������л�ȡ�����̳߳��е��̵߳��ã�
	void Pop(T& task)
	{
		task = _task_queue.front();
		_task_queue.pop();
	}
private:
	ThreadPool(int num = NUM) //���캯��˽��
		: _thread_num(num)
	{
		pthread_mutex_init(&_mutex, nullptr);
		pthread_cond_init(&_cond, nullptr);
	}
	ThreadPool(const ThreadPool&) = delete; //������

	std::queue<T> _task_queue; //�������
	int _thread_num; //�̳߳����̵߳�����
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;

	static ThreadPool<T> _sInst;
};

template<class T>
ThreadPool<T> ThreadPool<T>::_sInst;

//Task.hpp

#pragma once

#include <iostream>
#include "reactor.hpp"
#include "comm.hpp"

//������
class Task
{
private:
	struct data _d;
	EventItem* _item;
public:
	Task(struct data d, EventItem* item)
		: _d(d), _item(item)
	{}
	Task() //�ṩĬ�Ϲ���
	{}
	~Task()
	{}
	//��������ķ���
	void Run()
	{
		//4��ҵ����
		int result = 0;
		switch (_d._op)
		{
		case '+':
			result = _d._x + _d._y;
			break;
		case '-':
			result = _d._x - _d._y;
			break;
		case '*':
			result = _d._x * _d._y;
			break;
		case '/':
			if (_d._y == 0){
				std::cerr << "Error: div zero!" << std::endl;
				return;
			}
			else{
				result = _d._x / _d._y;
			}
			break;
		case '%':
			if (_d._y == 0){
				std::cerr << "Error: mod zero!" << std::endl;
				return;
			}
			else{
				result = _d._x % _d._y;
			}
			break;
		default:
			std::cerr << "operation error!" << std::endl;
			return;
		}
		std::cout << "thread[" << pthread_self() << "]:" << _d._x << _d._op << _d._y << "=" << result << std::endl;

		//5���γ���Ӧ����
		std::string response;
		response += std::to_string(_d._x);
		response += _d._op;
		response += std::to_string(_d._y);
		response += "=";
		response += std::to_string(result);
		response += "X"; //�����뱨��֮��ķָ���
		
		//6������Ӧ������ӵ�outbuffer��
		_item->_outbuffer += response;
		if (!_item->_outbuffer.empty())
			_item->_R->EnableReadWrite(_item->_sock, true, true); //��д�¼�
	}
};

//recver

int recver(EventItem* item)
{
	if (item->_sock < 0) //���ļ��������Ѿ����ر�
		return -1;

	//1�����ݶ�ȡ
	if (recver_helper(item->_sock, &(item->_inbuffer)) < 0){ //��ȡʧ��
		item->_error_handler(item);
		return -1;
	}

	//2�������и�
	std::vector<std::string> datagrams;
	StringUtil::Split(item->_inbuffer, &datagrams, "X");

	//3�������л�
	for (auto s : datagrams){
		struct data d;
		StringUtil::Deserialize(s, &d._x, &d._y, &d._op);

		Task t(d, item); //��������
		ThreadPool<Task>::GetInstance()->Push(t); //������push���̳߳ص����������
	}
	return 0;
}

//��ʼ���̳߳�
ThreadPool<Task>::GetInstance()->ThreadPoolInit();