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

#include <stdio.h>
int main()
{
	int tickets = 1000;
	--tickets;
	return 0;
}