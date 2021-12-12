#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
	printf("get a signal:%d\n", sig);
}

int main()
{
	signal(2, handler);
	while (1){
		printf("hello signal!\n");
		sleep(1);
	}
	return 0;
}

typedef void(*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);