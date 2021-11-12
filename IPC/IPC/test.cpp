//int pipe(int pipefd[2]);

//child->write, father->read                                                                                                                                                                                                                                                                                                                                                                                                                                                        
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
	int fd[2] = { 0 };
	if (pipe(fd) < 0){ //使用pipe创建匿名管道
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //使用fork创建子进程
	if (id == 0){
		//child
		close(fd[0]); //子进程关闭读端
		//子进程向管道写入数据
		const char* msg = "hello father, I am child...";
		int count = 10;
		while (count--){
			write(fd[1], msg, strlen(msg));
			sleep(1);
		}
		close(fd[1]); //子进程写入完毕，关闭文件
		exit(0);
	}
	//father
	close(fd[1]); //父进程关闭写端
	//父进程从管道读取数据
	char buff[64];
	while (1){
		ssize_t s = read(fd[0], buff, sizeof(buff));
		if (s > 0){
			buff[s] = '\0';
			printf("child send to father:%s\n", buff);
		}
		else if (s == 0){
			printf("read file end\n");
			break;
		}
		else{
			printf("read error\n");
			break;
		}
	}
	close(fd[0]); //父进程读取完毕，关闭文件
	waitpid(id, NULL, 0);
	return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
	int fd[2] = { 0 };
	if (pipe(fd) < 0){ //使用pipe创建匿名管道
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //使用fork创建子进程
	if (id == 0){
		//child
		close(fd[0]); //子进程关闭读端
		//子进程向管道写入数据
		const char* msg = "hello father, I am child...";
		int count = 10;
		while (count--){
			write(fd[1], msg, strlen(msg));
			sleep(1);
		}
		close(fd[1]); //子进程写入完毕，关闭文件
		exit(0);
	}
	//father
	close(fd[1]); //父进程关闭写端
	close(fd[0]); //父进程直接关闭读端（导致子进程被操作系统杀掉）
	int status = 0;
	waitpid(id, &status, 0);
	printf("child get signal:%d\n", status & 0x7F); //打印子进程收到的信号
	return 0;
}

//查看管道大小
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int main()
{
	int fd[2] = { 0 };
	if (pipe(fd) < 0){ //使用pipe创建匿名管道
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //使用fork创建子进程
	if (id == 0){
		//child 
		close(fd[0]); //子进程关闭读端
		char c = 'a';
		int count = 0;
		//子进程一直进行写入，一次写入一个字节
		while (1){
			write(fd[1], &c, 1);
			count++;
			printf("%d\n", count); //打印当前写入的字节数
		}
		close(fd[1]);
		exit(0);
	}
	//father
	close(fd[1]); //父进程关闭写端

	//父进程不进行读取

	waitpid(id, NULL, 0);
	close(fd[0]);
	return 0;
}

int pipe2(int pipefd[2], int flags);

int mkfifo(const char *pathname, mode_t mode);

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILE_NAME "myfifo"

int main()
{
	umask(0); //将文件默认掩码设置为0
	if (mkfifo(FILE_NAME, 0666) < 0){
		perror("mkfifo");
		return 1;
	}

	//create success...

	return 0;
}

//server.c
#include "comm.h"

int main()
{
	umask(0); //将文件默认掩码设置为0
	if (mkfifo(FILE_NAME, 0666) < 0){
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //打开命名管道文件
	if (fd < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //每次读之前将msg清空
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //手动设置'\0'，便于输出
			printf("client# %s", msg);
			fflush(stdout);
		}
		else if (s == 0){
			printf("client quit!\n");
			break;
		}
		else{
			printf("read error!\n");
			break;
		}
	}
	close(fd); //读取完毕，关闭文件
	return 0;
}

//client.c
#include "comm.h"

int main()
{
	int fd = open(FILE_NAME, O_WRONLY);
	if (fd < 0){
		perror("open");
		return 1;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //将msg清空
		printf("Please Enter# ");
		fflush(stdout);
		ssize_t s = read(0, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0';
			write(fd, msg, strlen(msg));
		}
	}
	close(fd);
	return 0;
}

//comm.h
#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#define FILE_NAME "myfifo"
