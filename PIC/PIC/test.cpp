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
