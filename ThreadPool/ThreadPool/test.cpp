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
			//sleep(1);
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


#pragma once

#include <iostream>

//任务类
class Task
{
public:
	Task(int x = 0, int y = 0, char op = 0)
		: _x(x), _y(y), _op(op)
	{}
	~Task()
	{}

	//处理任务的方法
	void Run()
	{
		int result = 0;
		switch (_op)
		{
		case '+':
			result = _x + _y;
			break;
		case '-':
			result = _x - _y;
			break;
		case '*':
			result = _x * _y;
			break;
		case '/':
			if (_y == 0){
				std::cerr << "Error: div zero!" << std::endl;
				return;
			}
			else{
				result = _x / _y;
			}
			break;
		case '%':
			if (_y == 0){
				std::cerr << "Error: mod zero!" << std::endl;
				return;
			}
			else{
				result = _x % _y;
			}
			break;
		default:
			std::cerr << "operation error!" << std::endl;
			return;
		}
		std::cout << "thread[" << pthread_self() << "]:" << _x << _op << _y << "=" << result << std::endl;
	}
private:
	int _x;
	int _y;
	char _op;
};


#include "Task.hpp"
#include "ThreadPool.hpp"

int main()
{
	srand((unsigned int)time(nullptr));
	ThreadPool<Task>* tp = new ThreadPool<Task>; //线程池
	tp->ThreadPoolInit(); //初始化线程池当中的线程
	const char* op = "+-*/%";
	//不断往任务队列塞计算任务
	while (true){
		sleep(1);
		int x = rand() % 100;
		int y = rand() % 100;
		int index = rand() % 5;
		Task task(x, y, op[index]);
		tp->Push(task);
	}
	return 0;
}
