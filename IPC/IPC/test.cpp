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
	if (mkfifo(FILE_NAME, 0666) < 0){ //使用mkfifo创建命名管道文件
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //以读的方式打开命名管道文件
	if (fd < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //每次读之前将msg清空
		//从命名管道当中读取信息
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //手动设置'\0'，便于输出
			printf("client# %s\n", msg); //输出客户端发来的信息
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
	close(fd); //通信完毕，关闭命名管道文件
	return 0;
}

//client.c
#include "comm.h"

int main()
{
	int fd = open(FILE_NAME, O_WRONLY); //以写的方式打开命名管道文件
	if (fd < 0){
		perror("open");
		return 1;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //每次读之前将msg清空
		printf("Please Enter# "); //提示客户端输入
		fflush(stdout);
		//从客户端的标准输入流读取信息
		ssize_t s = read(0, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s - 1] = '\0';
			//将信息写入命名管道
			write(fd, msg, strlen(msg));
		}
	}
	close(fd); //通信完毕，关闭命名管道文件
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

#define FILE_NAME "myfifo" //让客户端和服务端使用同一个命名管道


//派发计算任务
//server.c
#include "comm.h"

int main()
{
	umask(0); //将文件默认掩码设置为0
	if (mkfifo(FILE_NAME, 0666) < 0){ //使用mkfifo创建命名管道文件
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
		//从命名管道当中读取信息
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //手动设置'\0'，便于输出
			printf("client# %s\n", msg);
			//服务端进行计算任务
		    char* lable = "+-*/%";
			char* p = msg;
			int flag = 0;
			while (*p){
				switch (*p){
				case '+':
					flag = 0;
					break;
				case '-':
					flag = 1;
					break;
				case '*':
					flag = 2;
					break;
				case '/':
					flag = 3;
					break;
				case '%':
					flag = 4;
					break;
				}
				p++;
			}
			char* data1 = strtok(msg, "+-*/%");
			char* data2 = strtok(NULL, "+-*/%");
			int num1 = atoi(data1);
			int num2 = atoi(data2);
			int ret = 0;
			switch (flag){
			case 0:
				ret = num1 + num2;
				break;
			case 1:
				ret = num1 - num2;
				break;
			case 2:
				ret = num1 * num2;
				break;
			case 3:
				ret = num1 / num2;
				break;
			case 4:
				ret = num1 % num2;
				break;
			}
			printf("%d %c %d = %d\n", num1, lable[flag], num2, ret); //打印计算结果
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
	close(fd); //通信完毕，关闭命名管道文件
	return 0;
}


//文件拷贝
//server.c
#include "comm.h"

int main()
{
	umask(0); //将文件默认掩码设置为0
	if (mkfifo(FILE_NAME, 0666) < 0){ //使用mkfifo创建命名管道文件
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //以读的方式打开命名管道文件
	if (fd < 0){
		perror("open");
		return 2;
	}
	//创建文件file-bat.txt，并以写的方式打开该文件
	int fdout = open("file-bat.txt", O_CREAT | O_WRONLY, 0666);
	if (fdout < 0){
		perror("open");
		return 3;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //每次读之前将msg清空
		//从命名管道当中读取信息
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			write(fdout, msg, s); //将读取到的信息写入到file-bat.txt文件当中
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
	close(fd); //通信完毕，关闭命名管道文件
	close(fdout); //数据写入完毕，关闭file-bat.txt文件
	return 0;
}

//client.c
#include "comm.h"

int main()
{
	int fd = open(FILE_NAME, O_WRONLY); //以写的方式打开命名管道文件
	if (fd < 0){
		perror("open");
		return 1;
	}
	int fdin = open("file.txt", O_RDONLY); //以读的方式打开file.txt文件
	if (fdin < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		//从file.txt文件当中读取数据
		ssize_t s = read(fdin, msg, sizeof(msg));
		if (s > 0){
			write(fd, msg, s); //将读取到的数据写入到命名管道当中
		}
		else if (s == 0){
			printf("read end of file!\n");
			 break;
		}
		else{
			printf("read error!\n");
			break;
		}
	}
	close(fd); //通信完毕，关闭命名管道文件
	close(fdin); //数据读取完毕，关闭file.txt文件
	return 0;
}

//遥控进程
#include "comm.h"

int main()
{
	umask(0); //将文件默认掩码设置为0
	if (mkfifo(FILE_NAME, 0666) < 0){ //使用mkfifo创建命名管道文件
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //以读的方式打开命名管道文件
	if (fd < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //每次读之前将msg清空
		//从命名管道当中读取信息
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //手动设置'\0'，便于输出
			printf("client# %s\n", msg);
			if (fork() == 0){
				//child
				execlp(msg, msg, NULL); //进程程序替换
				exit(1);
			}
			waitpid(-1, NULL, 0); //等待子进程
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
	close(fd); //通信完毕，关闭命名管道文件
	return 0;
}
