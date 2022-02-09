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
