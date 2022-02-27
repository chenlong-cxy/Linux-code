#pragma once

#include <iostream>
#include <pthread.h>
#include <queue>
#include <unistd.h>

#define NUM 5

template<class T>
class BlockQueue
{
private:
	bool IsFull()
	{
		return _q.size() == _cap;
	}
	bool IsEmpty()
	{
		return _q.empty();
	}
public:
	BlockQueue(int cap = NUM)
		: _cap(cap)
	{
		pthread_mutex_init(&_mutex, nullptr);
		pthread_cond_init(&_full, nullptr);
		pthread_cond_init(&_empty, nullptr);
	}
	~BlockQueue()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_full);
		pthread_cond_destroy(&_empty);
	}
	//���������в������ݣ������ߵ��ã�
	void Push(const T& data)
	{
		pthread_mutex_lock(&_mutex);
		while (IsFull()){
			//���ܽ���������ֱ���������п��������µ�����
			pthread_cond_wait(&_full, &_mutex);
		}
		_q.push(data);
		pthread_mutex_unlock(&_mutex);
		pthread_cond_signal(&_empty); //������empty���������µȴ����������߳�
	}
	//���������л�ȡ���ݣ������ߵ��ã�
	void Pop(T& data)
	{
		pthread_mutex_lock(&_mutex);
		while (IsEmpty()){
			//���ܽ������ѣ�ֱ�������������µ�����
			pthread_cond_wait(&_empty, &_mutex);
		}
		data = _q.front();
		_q.pop();
		pthread_mutex_unlock(&_mutex);
		pthread_cond_signal(&_full); //������full���������µȴ����������߳�
	}
private:
	std::queue<T> _q; //��������
	int _cap; //������������������ݸ���
	pthread_mutex_t _mutex;
	pthread_cond_t _full;
	pthread_cond_t _empty;
};


#include "BlockQueue.hpp"

void* Producer(void* arg)
{
	BlockQueue<int>* bq = (BlockQueue<int>*)arg;
	//�����߲��Ͻ�������
	while (true){
		sleep(1);
		int data = rand() % 100 + 1;
		bq->Push(data); //��������
		std::cout << "Producer: " << data << std::endl;
	}
}
void* Consumer(void* arg)
{
	BlockQueue<int>* bq = (BlockQueue<int>*)arg;
	//�����߲��Ͻ�������
	while (true){
		sleep(1);
		int data = 0;
		bq->Pop(data); //��������
		std::cout << "Consumer: " << data << std::endl;
	}
}
int main()
{
	srand((unsigned int)time(nullptr));
	pthread_t producer, consumer;
	BlockQueue<int>* bq = new BlockQueue<int>;
	//�����������̺߳��������߳�
	pthread_create(&producer, nullptr, Producer, bq);
	pthread_create(&consumer, nullptr, Consumer, bq);

	//join�������̺߳��������߳�
	pthread_join(producer, nullptr);
	pthread_join(consumer, nullptr);
	delete bq
	return 0;
}


//���������в������ݣ������ߵ��ã�
void Push(const T& data)
{
	pthread_mutex_lock(&_mutex);
	while (IsFull()){
		//���ܽ���������ֱ���������п��������µ�����
		pthread_cond_wait(&_full, &_mutex);
	}
	_q.push(data);
	if (_q.size() >= _cap / 2){
		pthread_cond_signal(&_empty); //������empty���������µȴ����������߳�
	}
	pthread_mutex_unlock(&_mutex);
}
//���������л�ȡ���ݣ������ߵ��ã�
void Pop(T& data)
{
	pthread_mutex_lock(&_mutex);
	while (IsEmpty()){
		//���ܽ������ѣ�ֱ�������������µ�����
		pthread_cond_wait(&_empty, &_mutex);
	}
	data = _q.front();
	_q.pop();
	if (_q.size() <= _cap / 2){
		pthread_cond_signal(&_full); //������full���������µȴ����������߳�
	}
	pthread_mutex_unlock(&_mutex);
}

#pragma once
#include <iostream>

class Task
{
public:
	Task(int x = 0, int y = 0, int op = 0)
		: _x(x), _y(y), _op(op)
	{}
	~Task()
	{}
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
				std::cout << "Warning: div zero!" << std::endl;
				result = -1;
			}
			else{
				result = _x / _y;
			}
			break;
		case '%':
			if (_y == 0){
				std::cout << "Warning: mod zero!" << std::endl;
				result = -1;
			}
			else{
				result = _x % _y;
			}
			break;
		default:
			std::cout << "error operation!" << std::endl;
			break;
		}
		std::cout << _x << _op << _y << "=" << result << std::endl;
	}
private:
	int _x;
	int _y;
	char _op;
};


void* Producer(void* arg)
{
	BlockQueue<Task>* bq = (BlockQueue<Task>*)arg;
	const char* arr = "+-*/%";
	//�����߲��Ͻ�������
	while (true){
		int x = rand() % 100;
		int y = rand() % 100;
		char op = arr[rand() % 5];
		Task t(x, y, op);
		bq->Push(t); //��������
		std::cout << "producer task done" << std::endl;
	}
}
void* Consumer(void* arg)
{
	BlockQueue<Task>* bq = (BlockQueue<Task>*)arg;
	//�����߲��Ͻ�������
	while (true){
		sleep(1);
		Task t;
		bq->Pop(t); //��������
		t.Run(); //��������
	}
}


