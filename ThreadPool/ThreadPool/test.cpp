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
	//�̳߳����̵߳�ִ������
	static void* Routine(void* arg)
	{
		pthread_detach(pthread_self());
		ThreadPool* self = (ThreadPool*)arg;
		//���ϴ�������л�ȡ������д���
		while (true){
			//sleep(1);
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
	std::queue<T> _task_queue; //�������
	int _thread_num; //�̳߳����̵߳�����
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};


#pragma once

#include <iostream>

//������
class Task
{
public:
	Task(int x = 0, int y = 0, char op = 0)
		: _x(x), _y(y), _op(op)
	{}
	~Task()
	{}

	//��������ķ���
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
	ThreadPool<Task>* tp = new ThreadPool<Task>; //�̳߳�
	tp->ThreadPoolInit(); //��ʼ���̳߳ص��е��߳�
	const char* op = "+-*/%";
	//�����������������������
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
