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

#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
	unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

typedef __sigset_t sigset_t;


#include <signal.h>

int sigemptyset(sigset_t *set);

int sigfillset(sigset_t *set);

int sigaddset(sigset_t *set, int signum);

int sigdelset(sigset_t *set, int signum);

int sigismember(const sigset_t *set, int signum);

#include <stdio.h>
#include <signal.h>

int main()
{
	sigset_t s; //用户空间定义的变量

	sigemptyset(&s);

	sigfillset(&s);

	sigaddset(&s, SIGINT);

	sigdelset(&s, SIGINT);

	sigismember(&s, SIGINT);
	return 0;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int sigpending(sigset_t *set);

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void printPending(sigset_t *pending)
{
	int i = 1;
	for (i = 1; i <= 31; i++){
		if (sigismember(pending, i)){
			printf("1 ");
		}
		else{
			printf("0 ");
		}
	}
	printf("\n");
}
int main()
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigemptyset(&oset);

	sigaddset(&set, 2); //SIGINT
	sigprocmask(SIG_SETMASK, &set, &oset); //阻塞2号信号

	sigset_t pending;
	sigemptyset(&pending);

	while (1){
		sigpending(&pending); //获取pending
		printPending(&pending); //打印pending位图（1表示未决）
		sleep(1);
	}
	return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void printPending(sigset_t *pending)
{
	int i = 1;
	for (i = 1; i <= 31; i++){
		if (sigismember(pending, i)){
			printf("1 ");
		}
		else{
			printf("0 ");
		}
	}
	printf("\n");
}
void handler(int signo)
{
	printf("handler signo:%d\n", signo);
}
int main()
{
	signal(2, handler);
	sigset_t set, oset;
	sigemptyset(&set);
	sigemptyset(&oset);

	sigaddset(&set, 2); //SIGINT
	sigprocmask(SIG_SETMASK, &set, &oset); //阻塞2号信号

	sigset_t pending;
	sigemptyset(&pending);

	int count = 0;
	while (1){
		sigpending(&pending); //获取pending
		printPending(&pending); //打印pending位图（1表示未决）
		sleep(1);
		count++;
		if (count == 20){
			sigprocmask(SIG_SETMASK, &oset, NULL); //恢复曾经的信号屏蔽字
			printf("恢复信号屏蔽字\n");
		}
	}
	return 0;
}


int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

struct sigaction {
	void(*sa_handler)(int);
	void(*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t   sa_mask;
	int        sa_flags;
	void(*sa_restorer)(void);
};

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

struct sigaction act, oact;
void handler(int signo)
{
	printf("get a signal:%d\n", signo);
	sigaction(2, &oact, NULL);
}
int main()
{
	memset(&act, 0, sizeof(act));
	memset(&oact, 0, sizeof(oact));

	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	sigaction(2, &act, &oact);
	while (1){
		printf("I am a process...\n");
		sleep(1);
	}
	return 0;
}

#include <stdio.h>
#include <signal.h>

volatile int flag = 0;

void handler(int signo)
{
	printf("get a signal:%d\n", signo);
	flag = 1;
}
int main()
{
	signal(2, handler);
	while (!flag);
	printf("Proc Normal Quit!\n");
	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

void handler(int signo)
{
	printf("get a signal: %d\n", signo);
	int ret = 0;
	while ((ret = waitpid(-1, NULL, WNOHANG)) > 0){
		printf("wait child %d success\n", ret);
	}
}
int main()
{
	signal(SIGCHLD, handler);
	if (fork() == 0){
		//child
		printf("child is running, begin dead: %d\n", getpid());
		sleep(3);
		exit(1);
	}
	//father
	while (1);
	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main()
{
	signal(SIGCHLD, SIG_IGN);
	if (fork() == 0){
		//child
		printf("child is running, child dead: %d\n", getpid());
		sleep(3);
		exit(1);
	}
	//father
	while (1);
	return 0;
}
