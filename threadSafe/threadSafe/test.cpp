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

