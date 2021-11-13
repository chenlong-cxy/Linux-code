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
	if (mkfifo(FILE_NAME, 0666) < 0){ //ʹ��mkfifo���������ܵ��ļ�
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //�Զ��ķ�ʽ�������ܵ��ļ�
	if (fd < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //ÿ�ζ�֮ǰ��msg���
		//�������ܵ����ж�ȡ��Ϣ
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //�ֶ�����'\0'���������
			printf("client# %s\n", msg); //����ͻ��˷�������Ϣ
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
	close(fd); //ͨ����ϣ��ر������ܵ��ļ�
	return 0;
}

//client.c
#include "comm.h"

int main()
{
	int fd = open(FILE_NAME, O_WRONLY); //��д�ķ�ʽ�������ܵ��ļ�
	if (fd < 0){
		perror("open");
		return 1;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //ÿ�ζ�֮ǰ��msg���
		printf("Please Enter# "); //��ʾ�ͻ�������
		fflush(stdout);
		//�ӿͻ��˵ı�׼��������ȡ��Ϣ
		ssize_t s = read(0, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s - 1] = '\0';
			//����Ϣд�������ܵ�
			write(fd, msg, strlen(msg));
		}
	}
	close(fd); //ͨ����ϣ��ر������ܵ��ļ�
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

#define FILE_NAME "myfifo" //�ÿͻ��˺ͷ����ʹ��ͬһ�������ܵ�


//�ɷ���������
//server.c
#include "comm.h"

int main()
{
	umask(0); //���ļ�Ĭ����������Ϊ0
	if (mkfifo(FILE_NAME, 0666) < 0){ //ʹ��mkfifo���������ܵ��ļ�
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
		//�������ܵ����ж�ȡ��Ϣ
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //�ֶ�����'\0'���������
			printf("client# %s\n", msg);
			//����˽��м�������
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
			printf("%d %c %d = %d\n", num1, lable[flag], num2, ret); //��ӡ������
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
	close(fd); //ͨ����ϣ��ر������ܵ��ļ�
	return 0;
}


//�ļ�����
//server.c
#include "comm.h"

int main()
{
	umask(0); //���ļ�Ĭ����������Ϊ0
	if (mkfifo(FILE_NAME, 0666) < 0){ //ʹ��mkfifo���������ܵ��ļ�
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //�Զ��ķ�ʽ�������ܵ��ļ�
	if (fd < 0){
		perror("open");
		return 2;
	}
	//�����ļ�file-bat.txt������д�ķ�ʽ�򿪸��ļ�
	int fdout = open("file-bat.txt", O_CREAT | O_WRONLY, 0666);
	if (fdout < 0){
		perror("open");
		return 3;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //ÿ�ζ�֮ǰ��msg���
		//�������ܵ����ж�ȡ��Ϣ
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			write(fdout, msg, s); //����ȡ������Ϣд�뵽file-bat.txt�ļ�����
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
	close(fd); //ͨ����ϣ��ر������ܵ��ļ�
	close(fdout); //����д����ϣ��ر�file-bat.txt�ļ�
	return 0;
}

//client.c
#include "comm.h"

int main()
{
	int fd = open(FILE_NAME, O_WRONLY); //��д�ķ�ʽ�������ܵ��ļ�
	if (fd < 0){
		perror("open");
		return 1;
	}
	int fdin = open("file.txt", O_RDONLY); //�Զ��ķ�ʽ��file.txt�ļ�
	if (fdin < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		//��file.txt�ļ����ж�ȡ����
		ssize_t s = read(fdin, msg, sizeof(msg));
		if (s > 0){
			write(fd, msg, s); //����ȡ��������д�뵽�����ܵ�����
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
	close(fd); //ͨ����ϣ��ر������ܵ��ļ�
	close(fdin); //���ݶ�ȡ��ϣ��ر�file.txt�ļ�
	return 0;
}

//ң�ؽ���
#include "comm.h"

int main()
{
	umask(0); //���ļ�Ĭ����������Ϊ0
	if (mkfifo(FILE_NAME, 0666) < 0){ //ʹ��mkfifo���������ܵ��ļ�
		perror("mkfifo");
		return 1;
	}
	int fd = open(FILE_NAME, O_RDONLY); //�Զ��ķ�ʽ�������ܵ��ļ�
	if (fd < 0){
		perror("open");
		return 2;
	}
	char msg[128];
	while (1){
		msg[0] = '\0'; //ÿ�ζ�֮ǰ��msg���
		//�������ܵ����ж�ȡ��Ϣ
		ssize_t s = read(fd, msg, sizeof(msg)-1);
		if (s > 0){
			msg[s] = '\0'; //�ֶ�����'\0'���������
			printf("client# %s\n", msg);
			if (fork() == 0){
				//child
				execlp(msg, msg, NULL); //���̳����滻
				exit(1);
			}
			waitpid(-1, NULL, 0); //�ȴ��ӽ���
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
	close(fd); //ͨ����ϣ��ر������ܵ��ļ�
	return 0;
}
