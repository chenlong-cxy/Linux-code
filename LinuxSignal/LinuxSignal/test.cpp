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

#include <stdio.h>
#include <unistd.h>

int main()
{
	while (1){
		printf("hello signal!\n");
		sleep(1);
	}
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	if (fork() == 0){
		//child
		printf("I am running...\n");
		int *p = NULL;
		*p = 100;
		exit(0);
	}
	//father
	int status = 0;
	waitpid(-1, &status, 0);
	printf("exitCode:%d, coreDump:%d, signal:%d\n",
		(status >> 8) & 0xff, (status >> 7) & 1, status & 0x7f);
	return 0;
}

/home/cl/.local/share/nvim/swap//%home%cl%Linuxcode%SIG%sigGenera%myproc.c.swp

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int signal)
{
	printf("get a signal:%d\n", signal);
}
int main()
{
	int signo;
	for (signo = 1; signo <= 31; signo++){
		signal(signo, handler);
	}
	while (1){
		sleep(1);
	}
	return 0;
}

int kill(pid_t pid, int sig);

1 #include <stdio.h>
2 #include <stdlib.h>
3 #include <sys/types.h>
4 #include <signal.h>
5
6 void Usage(char* proc)
7 {
	8     printf("Usage: %s pid signo\n", proc);
	9 }
10 int main(int argc, char* argv[])
11 {
	12     if (argc != 3){
		13         Usage(argv[0]);
		14         return 1;
		15
	}
	16     pid_t pid = atoi(argv[1]);
	17     int signo = atoi(argv[2]);
	18     kill(pid, signo);
	19     return 0;
	20 }
