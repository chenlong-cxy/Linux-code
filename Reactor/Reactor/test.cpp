//reactor.hpp

#pragma once

#include "comm.hpp"
#include <unistd.h>
#include <sys/epoll.h>
#include <string>
#include <unordered_map>

#define SIZE 256
#define MAX_NUM 64

//前置声明
class Reactor;
class EventItem;

typedef int(*callback_t)(EventItem*);

class EventItem{
public:
	int _sock; //文件描述符
	Reactor* _R; //回指指针

	callback_t _recv_handler; //读回调
	callback_t _send_handler; //写回调
	callback_t _error_handler; //异常回调

	std::string _inbuffer; //输入缓冲区
	std::string _outbuffer; //输出缓冲区
public:
	EventItem()
		: _sock(-1)
		, _R(nullptr)
		, _recv_handler(nullptr)
		, _send_handler(nullptr)
		, _error_handler(nullptr)
	{}
	//管理回调
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
	int _epfd; //epoll模型
	std::unordered_map<int, EventItem> _event_items; //建立sock与EventItem结构的映射
public:
	Reactor()
		: _epfd(-1)
	{}
	void InitReactor()
	{
		//创建epoll模型
		_epfd = epoll_create(SIZE);
		if (_epfd < 0){
			std::cerr << "epoll_create error" << std::endl;
			exit(5);
		}
	}
	//事件分派器
	void Dispatcher(int timeout)
	{
		struct epoll_event revs[MAX_NUM];
		int num = epoll_wait(_epfd, revs, MAX_NUM, timeout);
		for (int i = 0; i < num; i++){
			int sock = revs[i].data.fd; //就绪的文件描述符
			if ((revs[i].events&EPOLLERR) || (revs[i].events&EPOLLHUP)){ //异常事件就绪（优先处理）
				if (_event_items[sock]._error_handler)
					_event_items[sock]._error_handler(&_event_items[sock]); //调用异常回调
			}
			if (revs[i].events&EPOLLIN){ //读事件就绪
				if (_event_items[sock]._recv_handler)
					_event_items[sock]._recv_handler(&_event_items[sock]); //调用读回调
			}
			if (revs[i].events&EPOLLOUT){ //写事件就绪
				if (_event_items[sock]._send_handler)
					_event_items[sock]._send_handler(&_event_items[sock]); //调用写回调
			}
		}
	}
	void AddEvent(int sock, uint32_t event, const EventItem& item)
	{
		struct epoll_event ev;
		ev.data.fd = sock;
		ev.events = event;
		
		if (epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev) < 0){ //将该文件描述符添加到epoll模型当中
			std::cerr << "epoll_ctl add error, fd: " << sock << std::endl;
		}
		else{
			//建立sock与EventItem结构的映射关系
			_event_items.insert({ sock, item });
			std::cout << "添加: " << sock << " 到epoll模型中，成功" << std::endl;
		}
	}
	void DelEvent(int sock)
	{
		if (epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr) < 0){ //将该文件描述符从epoll模型中删除
			std::cerr << "epoll_ctl del error, fd: " << sock << std::endl;
		}
		else{
			//取消sock与EventItem结构的映射关系
			_event_items.erase(sock);
			std::cout << "从epoll模型中删除: " << sock << "，成功" << std::endl;
		}
	}
	void EnableReadWrite(int sock, bool read, bool write){
		struct epoll_event ev;
		ev.data.fd = sock;
		ev.events = (read ? EPOLLIN : 0) | (write ? EPOLLOUT : 0) | EPOLLET;
		if (epoll_ctl(_epfd, EPOLL_CTL_MOD, sock, &ev) < 0){ //修改该文件描述符所需要监视的事件
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

//前置声明
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
			if (errno == EAGAIN || errno == EWOULDBLOCK){ //并没有读取出错，只是底层没有连接了
				return 0;
			}
			else if (errno == EINTR){ //读取的过程被信号中断了
				continue;
			}
			else{ //获取连接失败
				std::cerr << "accept error" << std::endl;
				return -1;
			}
		}
		SetNonBlock(sock); //将该套接字设置为非阻塞
		//构建EventItem结构
		EventItem sock_item;
		sock_item._sock = sock;
		sock_item._R = item->_R;
		sock_item.ManageCallbacks(recver, sender, errorer); //注册回调方法
		
		Reactor* R = item->_R;
		R->AddEvent(sock, EPOLLIN | EPOLLET, sock_item); //将该套接字及其对应的事件注册到Dispatcher中
	}
	return 0;
}
int recver_helper(int sock, std::string* out)
{
	while (true){
		char buffer[128];
		ssize_t size = recv(sock, buffer, sizeof(buffer)-1, 0);
		if (size < 0){
			if (errno == EAGAIN || errno == EWOULDBLOCK){ //数据读取完毕
				return 0;
			}
			else if (errno == EINTR){ //被信号中断，继续尝试读取
				continue;
			}
			else{ //读取出错
				return -1;
			}
		}
		else if (size == 0){ //对端连接关闭
			return -1;
		}
		//读取成功
		buffer[size] = '\0';
		*out += buffer; //将读取到的数据添加到该套接字对应EventItem结构的inbuffer中
	}
}
int recver(EventItem* item)
{
	if (item->_sock < 0) //该文件描述符已经被关闭
		return -1;
	//1、数据读取
	if (recver_helper(item->_sock, &(item->_inbuffer)) < 0){ //读取失败
		item->_error_handler(item);
		return -1;
	}

	//2、报文切割
	std::vector<std::string> datagrams;
	StringUtil::Split(item->_inbuffer, &datagrams, "X");

	//3、反序列化
	for (auto s : datagrams){
		struct data d;
		StringUtil::Deserialize(s, &d._x, &d._y, &d._op);

		//4、业务处理
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
					continue; //继续处理下一个报文
				}
				else{
					result = d._x / d._y;
				}
				break;
			case '%':
				if (d._y == 0){
					std::cerr << "Error: mod zero!" << std::endl;
					continue; //继续处理下一个报文
				}
				else{
					result = d._x % d._y;
				}
				break;
			default:
				std::cerr << "operation error!" << std::endl;
				continue; //继续处理下一个报文
		}

		//5、形成响应报文
		std::string response;
		response += std::to_string(d._x);
		response += d._op;
		response += std::to_string(d._y);
		response += "=";
		response += std::to_string(result);
		response += "X"; //报文与报文之间的分隔符
		
		//6、将响应报文添加到outbuffer中
		item->_outbuffer += response;
		if (!item->_outbuffer.empty())
			item->_R->EnableReadWrite(item->_sock, true, true); //打开写事件
	}
	return 0;
}
int sender_helper(int sock, std::string& in)
{
	size_t total = 0; //累加已经发送的字节数
	while (true){
		ssize_t size = send(sock, in.c_str() + total, in.size() - total, 0);
		if (size < 0){
			if (errno == EAGAIN || errno == EWOULDBLOCK){ //底层发送缓冲区已经没有空间了
				in.erase(0, total); //将已经发送的数据移出outbuffer
				return 1; //缓冲区写满，没写完
			}
			else if (errno == EINTR){ //被信号中断，继续尝试写入
				continue;
			}
			else{ //写入出错
				return -1;
			}
		}
		total += size;
		if (total >= in.size()){
			in.clear(); //清空outbuffer
			return 0; //全部写入完毕
		}
	}
}
int sender(EventItem* item)
{
	if (item->_sock < 0) //该文件描述符已经被关闭
		return -1;

	int ret = sender_helper(item->_sock, item->_outbuffer);
	if (ret == 0){ //全部发送成功，不再关心写事件
		item->_R->EnableReadWrite(item->_sock, true, false);
	}
	else if (ret == 1){ //没有发送完毕，还需要继续关心写事件
		item->_R->EnableReadWrite(item->_sock, true, true);
	}
	else{ //写入出错
		item->_error_handler(item);
	}
	return 0;
}
int errorer(EventItem* item)
{
	item->_R->DelEvent(item->_sock); //将该文件描述符从epoll模型中删除，并取消该文件描述符与其EventItem结构的映射关系
	close(item->_sock); //关闭该文件描述符

	item->_sock = -1; //防止关闭后继续执行读写回调
	return 0;
}

//util.hpp

#pragma once

#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

//设置文件描述符为非阻塞
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

	//服务器监听套接字的创建、绑定和监听
	int listen_sock = Socket::SocketCreate();
	SetNonBlock(listen_sock); //将监听套接字设置为非阻塞
	Socket::SocketBind(listen_sock, port);
	Socket::SocketListen(listen_sock, BACK_LOG);
	
	//创建Reactor，并初始化
	Reactor R;
	R.InitReactor();

	//创建监听套接字对应的EventItem结构            
	EventItem item;
	item._sock = listen_sock;
	item._R = &R;
	item.ManageCallbacks(accepter, nullptr, nullptr); //监听套接字只需要关心读事件
	
	//将监听套接字托管给Dispatcher
	R.AddEvent(listen_sock, EPOLLIN | EPOLLET, item);
	
	//循环进行事件派发
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

 //线程池
template<class T>
class ThreadPool
{
public:
	//提供一个全局访问点
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
	ThreadPool(int num = NUM) //构造函数私有
		: _thread_num(num)
	{
		pthread_mutex_init(&_mutex, nullptr);
		pthread_cond_init(&_cond, nullptr);
	}
	ThreadPool(const ThreadPool&) = delete; //防拷贝

	std::queue<T> _task_queue; //任务队列
	int _thread_num; //线程池中线程的数量
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

//任务类
class Task
{
private:
	struct data _d;
	EventItem* _item;
public:
	Task(struct data d, EventItem* item)
		: _d(d), _item(item)
	{}
	Task() //提供默认构造
	{}
	~Task()
	{}
	//处理任务的方法
	void Run()
	{
		//4、业务处理
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

		//5、形成响应报文
		std::string response;
		response += std::to_string(_d._x);
		response += _d._op;
		response += std::to_string(_d._y);
		response += "=";
		response += std::to_string(result);
		response += "X"; //报文与报文之间的分隔符
		
		//6、将响应报文添加到outbuffer中
		_item->_outbuffer += response;
		if (!_item->_outbuffer.empty())
			_item->_R->EnableReadWrite(_item->_sock, true, true); //打开写事件
	}
};

//recver

int recver(EventItem* item)
{
	if (item->_sock < 0) //该文件描述符已经被关闭
		return -1;

	//1、数据读取
	if (recver_helper(item->_sock, &(item->_inbuffer)) < 0){ //读取失败
		item->_error_handler(item);
		return -1;
	}

	//2、报文切割
	std::vector<std::string> datagrams;
	StringUtil::Split(item->_inbuffer, &datagrams, "X");

	//3、反序列化
	for (auto s : datagrams){
		struct data d;
		StringUtil::Deserialize(s, &d._x, &d._y, &d._op);

		Task t(d, item); //构建任务
		ThreadPool<Task>::GetInstance()->Push(t); //将任务push到线程池的任务队列中
	}
	return 0;
}

//初始化线程池
ThreadPool<Task>::GetInstance()->ThreadPoolInit();