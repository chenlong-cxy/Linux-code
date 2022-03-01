//��ʼ���ź���
int sem_init(sem_t *sem, int pshared, unsigned int value);

//�����ź���
int sem_destroy(sem_t *sem);

//�ȴ��ź���
int sem_wait(sem_t *sem);

//�����ź���
int sem_post(sem_t *sem);


#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


class Sem{
public:
	Sem(int num)
	{
		sem_init(&_sem, 0, num);
	}
	~Sem()
	{
		sem_destroy(&_sem);
	}
	void P()
	{
		sem_wait(&_sem);
	}
	void V()
	{
		sem_post(&_sem);
	}
private:
	sem_t _sem;
};

Sem sem(1); //��Ԫ�ź���
int tickets = 2000;
void* TicketGrabbing(void* arg)
{
	std::string name = (char*)arg;
	while (true){
		sem.P();
		if (tickets > 0){
			usleep(1000);
			std::cout << name << " get a ticket, tickets left: " << --tickets << std::endl;
			sem.V();
		}
		else{
			sem.V();
			break;
		}
	}
	std::cout << name << " quit..." << std::endl;
	pthread_exit((void*)0);
}

int main()
{
	pthread_t tid1, tid2, tid3, tid4;
	pthread_create(&tid1, nullptr, TicketGrabbing, (void*)"thread 1");
	pthread_create(&tid2, nullptr, TicketGrabbing, (void*)"thread 2");
	pthread_create(&tid3, nullptr, TicketGrabbing, (void*)"thread 3");
	pthread_create(&tid4, nullptr, TicketGrabbing, (void*)"thread 4");

	pthread_join(tid1, nullptr);
	pthread_join(tid2, nullptr);
	pthread_join(tid3, nullptr);
	pthread_join(tid4, nullptr);
	return 0;
}

#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>

int tickets = 2000;
void* TicketGrabbing(void* arg)
{
	std::string name = (char*)arg;
	while (true){
		if (tickets > 0){
			usleep(1000);
			std::cout << name << " get a ticket, tickets left: " << --tickets << std::endl;
		}
		else{
			break;
		}
	}
	std::cout << name << " quit..." << std::endl;
	pthread_exit((void*)0);
}

int main()
{
	pthread_t tid1, tid2, tid3, tid4;
	pthread_create(&tid1, nullptr, TicketGrabbing, (void*)"thread 1");
	pthread_create(&tid2, nullptr, TicketGrabbing, (void*)"thread 2");
	pthread_create(&tid3, nullptr, TicketGrabbing, (void*)"thread 3");
	pthread_create(&tid4, nullptr, TicketGrabbing, (void*)"thread 4");
	
	pthread_join(tid1, nullptr);
	pthread_join(tid2, nullptr);
	pthread_join(tid3, nullptr);
	pthread_join(tid4, nullptr);
	return 0;
}



#pragma once

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

#define NUM 8

template<class T>
class RingQueue
{
private:
	//P����
	void P(sem_t& s)
	{
		sem_wait(&s);
	}
	//V����
	void V(sem_t& s)
	{
	    sem_post(&s);
	}
public:
	RingQueue(int cap = NUM)
		: _cap(cap), _p_pos(0), _c_pos(0)
	{
		_q.resize(_cap);
		sem_init(&_blank_sem, 0, _cap); //blank_sem��ʼֵ����Ϊ���ζ��е�����
		sem_init(&_data_sem, 0, 0); //data_sem��ʼֵ����Ϊ0
	}
	~RingQueue()
	{
	sem_destroy(&_blank_sem);
	sem_destroy(&_data_sem);
	}
	//���ζ��в������ݣ������ߵ��ã�
	void Push(const T& data)
	{
		P(_blank_sem); //�����߹�ע�ռ���Դ
		_q[_p_pos] = data;
		V(_data_sem); //����

		//������һ��������λ��
		_p_pos++;
		_p_pos %= _cap;
	}
	//�ӻ��ζ��л�ȡ���ݣ������ߵ��ã�
	void Pop(T& data)
	{
		P(_data_sem); //�����߹�ע������Դ
		data = _q[_c_pos];
		V(_blank_sem);

		//������һ�����ѵ�λ��
		_c_pos++;
		_c_pos %= _cap;
	}
private:
	std::vector<T> _q; //���ζ���
	int _cap; //���ζ��е���������
	int _p_pos; //����λ��
	int _c_pos; //����λ��
	sem_t _blank_sem; //�����ռ���Դ
	sem_t _data_sem; //����������Դ
};


#include "RingQueue.hpp"

void* Producer(void* arg)
{
	RingQueue<int>* rq = (RingQueue<int>*)arg;
	while (true){
		sleep(1);
		int data = rand() % 100 + 1;
		rq->Push(data);
		std::cout << "Producer: " << data << std::endl;
	}
}
void* Consumer(void* arg)
{
	RingQueue<int>* rq = (RingQueue<int>*)arg;
	while (true){
		sleep(1);
		int data = 0;
		rq->Pop(data);
		std::cout << "Consumer: " << data << std::endl;
	}
}
int main()
{
	srand((unsigned int)time(nullptr));
	pthread_t producer, consumer;
	RingQueue<int>* rq = new RingQueue<int>;
	pthread_create(&producer, nullptr, Producer, rq);
	pthread_create(&consumer, nullptr, Consumer, rq);
	
	pthread_join(producer, nullptr);
	pthread_join(consumer, nullptr);
	delete rq;
	return 0;
}
