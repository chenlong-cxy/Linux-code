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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

void Usage(char* proc)
{
	printf("Usage: %s pid signo\n", proc);
}
int main(int argc, char* argv[])
{
	if (argc != 3){
		Usage(argv[0]);
		return 1;
	}
	pid_t pid = atoi(argv[1]);
	int signo = atoi(argv[2]);
	kill(pid, signo);
	return 0;
}


int raise(int sig);

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int signo)
{
	printf("get a signal:%d\n", signo);
}
int main()
{
	signal(2, handler);
	while (1){
		sleep(1);
		raise(2);
	}
	return 0;
}

void abort(void);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int signo)
{
	printf("get a signal:%d\n", signo);
}


int main()
{
	signal(6, handler);
	while (1){
		sleep(1);
		//raise(2);
		abort();
	}
	return 0;
}


unsigned int alarm(unsigned int seconds);

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main()
{
	int count = 0;
	alarm(1);
	while (1){
		count++;
		printf("count: %d\n", count);
	}
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int count = 0;
void handler(int signo)
{
	printf("get a signal: %d\n", signo);
	printf("count: %d\n", count);
	exit(1);
}
int main()
{
	signal(SIGALRM, handler);
	alarm(1);
	while (1){
		count++;
	}
	return 0;
}
