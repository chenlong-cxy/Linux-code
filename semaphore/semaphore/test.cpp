//初始化信号量
int sem_init(sem_t *sem, int pshared, unsigned int value);

//销毁信号量
int sem_destroy(sem_t *sem);

//等待信号量
int sem_wait(sem_t *sem);

//发布信号量
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

Sem sem(1); //二元信号量
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


