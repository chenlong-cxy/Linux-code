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
	//向阻塞队列插入数据（生产者调用）
	void Push(const T& data)
	{
		pthread_mutex_lock(&_mutex);
		while (IsFull()){
			//不能进行生产，直到阻塞队列可以容纳新的数据
			pthread_cond_wait(&_full, &_mutex);
		}
		_q.push(data);
		pthread_mutex_unlock(&_mutex);
		pthread_cond_signal(&_empty); //唤醒在empty条件变量下等待的消费者线程
	}
	//从阻塞队列获取数据（消费者调用）
	void Pop(T& data)
	{
		pthread_mutex_lock(&_mutex);
		while (IsEmpty()){
			//不能进行消费，直到阻塞队列有新的数据
			pthread_cond_wait(&_empty, &_mutex);
		}
		data = _q.front();
		_q.pop();
		pthread_mutex_unlock(&_mutex);
		pthread_cond_signal(&_full); //唤醒在full条件变量下等待的生产者线程
	}
private:
	std::queue<T> _q; //阻塞队列
	int _cap; //阻塞队列最大容器数据个数
	pthread_mutex_t _mutex;
	pthread_cond_t _full;
	pthread_cond_t _empty;
};


#include "BlockQueue.hpp"

void* Producer(void* arg)
{
	BlockQueue<int>* bq = (BlockQueue<int>*)arg;
	//生产者不断进行生产
	while (true){
		sleep(1);
		int data = rand() % 100 + 1;
		bq->Push(data); //生产数据
		std::cout << "Producer: " << data << std::endl;
	}
}
void* Consumer(void* arg)
{
	BlockQueue<int>* bq = (BlockQueue<int>*)arg;
	//消费者不断进行消费
	while (true){
		sleep(1);
		int data = 0;
		bq->Pop(data); //消费数据
		std::cout << "Consumer: " << data << std::endl;
	}
}
int main()
{
	srand((unsigned int)time(nullptr));
	pthread_t producer, consumer;
	BlockQueue<int> bq;
	//创建生产者线程和消费者线程
	pthread_create(&producer, nullptr, Producer, &bq);
	pthread_create(&consumer, nullptr, Consumer, &bq);

	//join生产者线程和消费者线程
	pthread_join(producer, nullptr);
	pthread_join(consumer, nullptr);
	return 0;
}


//向阻塞队列插入数据（生产者调用）
void Push(const T& data)
{
	pthread_mutex_lock(&_mutex);
	while (IsFull()){
		//不能进行生产，直到阻塞队列可以容纳新的数据
		pthread_cond_wait(&_full, &_mutex);
	}
	_q.push(data);
	if (_q.size() >= _cap / 2){
		pthread_cond_signal(&_empty); //唤醒在empty条件变量下等待的消费者线程
	}
	pthread_mutex_unlock(&_mutex);
}
//从阻塞队列获取数据（消费者调用）
void Pop(T& data)
{
	pthread_mutex_lock(&_mutex);
	while (IsEmpty()){
		//不能进行消费，直到阻塞队列有新的数据
		pthread_cond_wait(&_empty, &_mutex);
	}
	data = _q.front();
	_q.pop();
	if (_q.size() <= _cap / 2){
		pthread_cond_signal(&_full); //唤醒在full条件变量下等待的生产者线程
	}
	pthread_mutex_unlock(&_mutex);
}
