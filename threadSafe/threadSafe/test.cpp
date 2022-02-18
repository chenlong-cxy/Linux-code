//#include <stdio.h>
//#include <pthread.h>
//#include <unistd.h>
//
//int count = 0;
//void* Routine(void* arg)
//{
//	while (1){
//		count++;
//		sleep(1);
//	}
//	pthread_exit((void*)0);
//}
//int main()
//{
//	pthread_t tid;
//	pthread_create(&tid, NULL, Routine, NULL);
//	while (1){
//		printf("count: %d\n", count);
//		sleep(1);
//	}
//	pthread_join(tid, NULL);
//	return 0;
//}
//
//#include <stdio.h>
//#include <unistd.h>
//#include <pthread.h>
//
//int tickets = 1000;
//void* TicketGrabbing(void* arg)
//{
//	const char* name = (char*)arg;
//	while (1){
//		if (tickets > 0){
//			usleep(10000);
//			printf("[%s] get a ticket, left: %d\n", name, --tickets);
//		}
//		else{
//			break;
//		}
//	}
//	printf("%s quit!\n", name);
//	pthread_exit((void*)0);
//}
//int main()
//{
//	pthread_t t1, t2, t3, t4;
//	pthread_create(&t1, NULL, TicketGrabbing, "thread 1");
//	pthread_create(&t2, NULL, TicketGrabbing, "thread 2");
//	pthread_create(&t3, NULL, TicketGrabbing, "thread 3");
//	pthread_create(&t4, NULL, TicketGrabbing, "thread 4");
//	
//	pthread_join(t1, NULL);
//	pthread_join(t2, NULL);
//	pthread_join(t3, NULL);
//	pthread_join(t4, NULL);
//	return 0;
//}

//#include <stdio.h>
//int main()
//{
//	int tickets = 1000;
//	--tickets;
//	return 0;
//}

//#include <stdio.h>
//int main()
//{
//	int tickets = 1000;
//	if (tickets > 0)
//	{
//		printf("%d\n", tickets);
//		tickets--;
//	}
//	return 0;
//}
//
//
////初始化互斥量
//int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);
//
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//
////销毁互斥量
//int pthread_mutex_destroy(pthread_mutex_t *mutex);
//
////互斥量加锁
//int pthread_mutex_lock(pthread_mutex_t *mutex);
//int pthread_mutex_trylock(pthread_mutex_t *mutex);
//
////互斥量解锁
//int pthread_mutex_unlock(pthread_mutex_t *mutex);
//
//#include <stdio.h>
//#include <unistd.h>
//#include <pthread.h>
//
//int tickets = 1000;
//pthread_mutex_t mutex;
//void* TicketGrabbing(void* arg)
//{
//	const char* name = (char*)arg;
//	while (1){
//		pthread_mutex_lock(&mutex);
//		if (tickets > 0){
//			usleep(100);
//			printf("[%s] get a ticket, left: %d\n", name, --tickets);
//			pthread_mutex_unlock(&mutex);
//		}
//		else{
//			pthread_mutex_unlock(&mutex);
//			break;
//		}
//	}
//	printf("%s quit!\n", name);
//	pthread_exit((void*)0);
//}
//int main()
//{
//	pthread_mutex_init(&mutex, NULL);
//	pthread_t t1, t2, t3, t4;
//	pthread_create(&t1, NULL, TicketGrabbing, "thread 1");
//	pthread_create(&t2, NULL, TicketGrabbing, "thread 2");
//	pthread_create(&t3, NULL, TicketGrabbing, "thread 3");
//	pthread_create(&t4, NULL, TicketGrabbing, "thread 4");
//
//	pthread_join(t1, NULL);
//	pthread_join(t2, NULL);
//	pthread_join(t3, NULL);
//	pthread_join(t4, NULL);
//	pthread_mutex_destroy(&mutex);
//	return 0;
//}

/*
lock:
	   movb $0, %al
	   xchgb %al, mutex
	   if (al寄存器的内容 > 0)
		   return 0;
	   else
		   挂起等待;
	   goto lock;
unlock:
	   movb $1, mutex
	   唤醒等待Mutex的线程;
	   return 0;
*/


//for (;;)
//{
//	//OS代码
//}

//#include <stdio.h>
//#include <pthread.h>
//
//pthread_mutex_t mutex;
//void* Routine(void* arg)
//{
//	pthread_mutex_lock(&mutex);
//	pthread_mutex_lock(&mutex);
//	
//	pthread_exit((void*)0);
//}
//int main()
//{
//	pthread_t tid;
//	pthread_mutex_init(&mutex, NULL);
//	pthread_create(&tid, NULL, Routine, NULL);
//	
//	pthread_join(tid, NULL);
//	pthread_mutex_destroy(&mutex);
//	return 0;
//}

//#include <iostream>
//using namespace std;
//typedef int(*pcal)(int, int);
//class Cal
//{
//public:
//	int Add(int x, int y)
//	{
//		return x + y;
//	}
//};
//int main()
//{
//	cout << &Cal::Add << endl;
//	printf("%p\n", &Cal::Add);
//	//pcal addr = &Cal::Add;
//	//printf("%p\n", addr(2,3));
//	return 0;
//}

//初始化条件变量
int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//销毁条件变量
int pthread_cond_destroy(pthread_cond_t *cond);

//等待条件变量满足
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);

//唤醒等待
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);

#include <iostream>
#include <cstdio>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
void* Routine(void* arg)
{
	pthread_detach(pthread_self());
	std::cout << (char*)arg << " run..." << std::endl;
	while (true){
		pthread_cond_wait(&cond, &mutex); //阻塞在这里，直到被唤醒
		std::cout << (char*)arg << "活动..." << std::endl;
	}
}
int main()
{
	pthread_t t1, t2, t3;
	pthread_mutex_init(&mutex, nullptr);
	pthread_cond_init(&cond, nullptr);
	
	pthread_create(&t1, nullptr, Routine, (void*)"thread 1");
	pthread_create(&t2, nullptr, Routine, (void*)"thread 2");
	pthread_create(&t3, nullptr, Routine, (void*)"thread 3");
	
	while (true){
		getchar();
		pthread_cond_signal(&cond);
		//pthread_cond_broadcast(&cond);
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}


//错误的设计
pthread_mutex_lock(&mutex);
while (condition_is_false){
	pthread_mutex_unlock(&mutex);
	//解锁之后，等待之前，条件可能已经满足，信号已经发出，但是该信号可能被错过
	pthread_cond_wait(&cond);
	pthread_mutex_lock(&mutex);
}
pthread_mutex_unlock(&mutex);


//等待条件代码
pthread_mutex_lock(&mutex);
while (条件为假)
	pthread_cond_wait(&cond, &mutex);
修改条件
pthread_mutex_unlock(&mutex);

//给条件发送信号代码
pthread_mutex_lock(&mutex);
设置条件为真
pthread_cond_signal(&cond);
pthread_mutex_unlock(&mutex);