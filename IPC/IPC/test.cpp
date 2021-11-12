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
	if (pipe(fd) < 0){ //ʹ��pipe���������ܵ�
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //ʹ��fork�����ӽ���
	if (id == 0){
		//child
		close(fd[0]); //�ӽ��̹رն���
		//�ӽ�����ܵ�д������
		const char* msg = "hello father, I am child...";
		int count = 10;
		while (count--){
			write(fd[1], msg, strlen(msg));
			sleep(1);
		}
		close(fd[1]); //�ӽ���д����ϣ��ر��ļ�
		exit(0);
	}
	//father
	close(fd[1]); //�����̹ر�д��
	//�����̴ӹܵ���ȡ����
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
	close(fd[0]); //�����̶�ȡ��ϣ��ر��ļ�
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
	if (pipe(fd) < 0){ //ʹ��pipe���������ܵ�
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //ʹ��fork�����ӽ���
	if (id == 0){
		//child
		close(fd[0]); //�ӽ��̹رն���
		//�ӽ�����ܵ�д������
		const char* msg = "hello father, I am child...";
		int count = 10;
		while (count--){
			write(fd[1], msg, strlen(msg));
			sleep(1);
		}
		close(fd[1]); //�ӽ���д����ϣ��ر��ļ�
		exit(0);
	}
	//father
	close(fd[1]); //�����̹ر�д��
	close(fd[0]); //������ֱ�ӹرն��ˣ������ӽ��̱�����ϵͳɱ����
	int status = 0;
	waitpid(id, &status, 0);
	printf("child get signal:%d\n", status & 0x7F); //��ӡ�ӽ����յ����ź�
	return 0;
}

//�鿴�ܵ���С
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int main()
{
	int fd[2] = { 0 };
	if (pipe(fd) < 0){ //ʹ��pipe���������ܵ�
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //ʹ��fork�����ӽ���
	if (id == 0){
		//child 
		close(fd[0]); //�ӽ��̹رն���
		char c = 'a';
		int count = 0;
		//�ӽ���һֱ����д�룬һ��д��һ���ֽ�
		while (1){
			write(fd[1], &c, 1);
			count++;
			printf("%d\n", count); //��ӡ��ǰд����ֽ���
		}
		close(fd[1]);
		exit(0);
	}
	//father
	close(fd[1]); //�����̹ر�д��

	//�����̲����ж�ȡ

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
	umask(0); //���ļ�Ĭ����������Ϊ0
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
	umask(0); //���ļ�Ĭ����������Ϊ0
	if (mkfifo(FILE_NAME, 0666) < 0){
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //�������ܵ��ļ�
	if (fd < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //ÿ�ζ�֮ǰ��msg���
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //�ֶ�����'\0'���������
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
	close(fd); //��ȡ��ϣ��ر��ļ�
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
		msg[0] = '\0'; //��msg���
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
