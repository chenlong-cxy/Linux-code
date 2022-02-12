pid_t vfork(void);

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int g_val = 100;
int main()
{
	pid_t id = vfork();
	if (id == 0){
		//child
		g_val = 200;
		printf("child:PID:%d, PPID:%d, g_val:%d\n", getpid(), getppid(), g_val);
		exit(0);
	}
	//father
	sleep(3);
	printf("father:PID:%d, PPID:%d, g_val:%d\n", getpid(), getppid(), g_val);
	return 0;
}

//线程创建
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	while (1){
		printf("I am %s\n", msg);
		sleep(1);
	}
}
int main()
{
	pthread_t tid;
	pthread_create(&tid, NULL, Routine, (void*)"thread 1");
	while (1){
		printf("I am main thread!\n");
		sleep(2);
	}
	return 0;
}

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	while (1){
		printf("I am %s...pid: %d, ppid: %d\n", msg, getpid(), getppid());
		sleep(1);
	}
}
int main()
{
	pthread_t tid;
	pthread_create(&tid, NULL, Routine, (void*)"thread 1");
	while (1){
		printf("I am main thread...pid: %d, ppid: %d\n", getpid(), getppid());
		sleep(2);
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	while (1){
		printf("I am %s...pid: %d, ppid: %d\n", msg, getpid(), getppid());
		sleep(1);
	}
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
	}
	while (1){
		printf("I am main thread...pid: %d, ppid: %d\n", getpid(), getppid());
		sleep(2);
	}
	return 0;
}

pthread_t pthread_self(void);

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	while (1){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
	}
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	while (1){
		 printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
		 sleep(2);
	}
	return 0;
}

//线程等待

int pthread_join(pthread_t thread, void **retval);

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
	}
	return NULL;
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
	for (int i = 0; i < 5; i++){
		pthread_join(tid[i], NULL);
		printf("thread %d[%lu]...quit\n", i, tid[i]);
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
		int a = 1 / 0; //error
	}
	return (void*)2022;
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
	for (int i = 0; i < 5; i++){
		void* ret = NULL;
		pthread_join(tid[i], &ret);
		printf("thread %d[%lu]...quit, exitcode: %d\n", i, tid[i], (int)ret);
	}
	return 0;
}


//线程终止

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	while (1){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
	}
	return (void*)0;
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());

	return 0;
}

void pthread_exit(void *retval);

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
	}
	pthread_exit((void*)6666);
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
	for (int i = 0; i < 5; i++){
		void* ret = NULL;
		pthread_join(tid[i], &ret);
		printf("thread %d[%lu]...quit, exitcode: %d\n", i, tid[i], (int)ret);
	}
	return 0;
}

int pthread_cancel(pthread_t thread);

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
		pthread_cancel(pthread_self());
	}
	pthread_exit((void*)6666);
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
	for (int i = 0; i < 5; i++){
		void* ret = NULL;
		pthread_join(tid[i], &ret);
		printf("thread %d[%lu]...quit, exitcode: %d\n", i, tid[i], (int)ret);
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
	}
	pthread_exit((void*)6666);
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	pthread_cancel(tid[0]);
	pthread_cancel(tid[1]);
	pthread_cancel(tid[2]);
	pthread_cancel(tid[3]);
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
	for (int i = 0; i < 5; i++){
		void* ret = NULL;
		pthread_join(tid[i], &ret);
		printf("thread %d[%lu]...quit, exitcode: %d\n", i, tid[i], (int)ret);
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

pthread_t main_thread;

void* Routine(void* arg)
{
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
		pthread_cancel(main_thread);
	}
	pthread_exit((void*)6666);
}
int main()
{
	main_thread = pthread_self();
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
	for (int i = 0; i < 5; i++){
		void* ret = NULL;
		pthread_join(tid[i], &ret);
		printf("thread %d[%lu]...quit, exitcode: %d\n", i, tid[i], (int)ret);
	}
	return 0;
}

//线程分离

int pthread_detach(pthread_t thread);

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void* Routine(void* arg)
{
	pthread_detach(pthread_self());
	char* msg = (char*)arg;
	int count = 0;
	while (count < 5){
		printf("I am %s...pid: %d, ppid: %d, tid: %lu\n", msg, getpid(), getppid(), pthread_self());
		sleep(1);
		count++;
	}
	pthread_exit((void*)6666);
}
int main()
{
	pthread_t tid[5];
	for (int i = 0; i < 5; i++){
		char* buffer = (char*)malloc(64);
		sprintf(buffer, "thread %d", i);
		pthread_create(&tid[i], NULL, Routine, buffer);
		printf("%s tid is %lu\n", buffer, tid[i]);
	}
	while (1){
		printf("I am main thread...pid: %d, ppid: %d, tid: %lu\n", getpid(), getppid(), pthread_self());
		sleep(1);
	}
	return 0;
}


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* Routine(void* arg)
{
	while (1){
		printf("new  thread tid: %p\n", pthread_self());
		sleep(1);
	}
}
int main()
{
	pthread_t tid;
	pthread_create(&tid, NULL, Routine, NULL);
	while (1){
		printf("main thread tid: %p\n", pthread_self());
		sleep(2);
	}
	return 0;
}
