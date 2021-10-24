////   /usr/include/stdio.h
//typedef struct _IO_FILE FILE;
//
////   /usr/include/libio.h
//struct _IO_FILE {
//	int _flags;       /* High-order word is _IO_MAGIC; rest is flags. */
//#define _IO_file_flags _flags
//
//	//缓冲区相关
//	/* The following pointers correspond to the C++ streambuf protocol. */
//	/* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
//	char* _IO_read_ptr;   /* Current read pointer */
//	char* _IO_read_end;   /* End of get area. */
//	char* _IO_read_base;  /* Start of putback+get area. */
//	char* _IO_write_base; /* Start of put area. */
//	char* _IO_write_ptr;  /* Current put pointer. */
//	char* _IO_write_end;  /* End of put area. */
//	char* _IO_buf_base;   /* Start of reserve area. */
//	char* _IO_buf_end;    /* End of reserve area. */
//	/* The following fields are used to support backing up and undo. */
//	char *_IO_save_base; /* Pointer to start of non-current get area. */
//	char *_IO_backup_base;  /* Pointer to first valid character of backup area */
//	char *_IO_save_end; /* Pointer to end of non-current get area. */
//
//	struct _IO_marker *_markers;
//
//	struct _IO_FILE *_chain;
//
//	int _fileno; //封装的文件描述符
//#if 0
//	int _blksize;
//#else
//	int _flags2;
//#endif
//	_IO_off_t _old_offset; /* This used to be _offset but it's too small.  */
//
//#define __HAVE_COLUMN /* temporary */
//	/* 1+column number of pbase(); 0 is unknown. */
//	unsigned short _cur_column;
//	signed char _vtable_offset;
//	char _shortbuf[1];
//
//	/*  char* _save_gptr;  char* _save_egptr; */
//
//	_IO_lock_t *_lock;
//#ifdef _IO_USE_OLD_IO_FILE
//};

//#include <stdio.h>
//#include <unistd.h>
//int main()
//{
//	//c                                                                                                    
//	printf("hello printf\n");
//	fputs("hello fputs\n", stdout);
//	//system
//	write(1, "hello write\n", 12);
//	fork();
//	return 0;
//}

#include <stdio.h>
int main()
{
	printf("hello printf\n"); //stdout
	perror("perror"); //stderr

	fprintf(stdout, "stdout:hello fprintf\n"); //stdout
	fprintf(stderr, "stderr:hello fprintf\n"); //stderr
	return 0;
}

int dup2(int oldfd, int newfd);

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
	int fd = open("log.txt", O_WRONLY | O_CREAT, 0666);
	if (fd < 0){
		perror("open");
		return 1;
	}
	close(1);
	dup2(fd, 1);
	printf("hello printf\n");
	fprintf(stdout, "hello fprintf\n");
	return 0;
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ⮂⮂ buffers
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define LEN 1024 //命令最大长度
#define NUM 32 //命令拆分后的最大个数
int main()
{
	int type = 0; //0 >, 1 >>, 2 <
	char cmd[LEN]; //存储命令
	char* myargv[NUM]; //存储命令拆分后的结果
	char hostname[32]; //主机名
	char pwd[128]; //当前目录
	while (1){
		//获取命令提示信息
		struct passwd* pass = getpwuid(getuid());
		gethostname(hostname, sizeof(hostname)-1);
		getcwd(pwd, sizeof(pwd)-1);
		int len = strlen(pwd);
		char* p = pwd + len - 1;
		while (*p != '/'){
			p--;
		}
		p++;
		//打印命令提示信息
		printf("[%s@%s %s]$ ", pass->pw_name, hostname, p);
		//读取命令
		fgets(cmd, LEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';

		//实现重定向功能
		char* start = cmd;
		while (*start != '\0'){
			if (*start == '>'){
				type = 0; //遇到一个'>'，输出重定向
				*start = '\0';
				start++;
				if (*start == '>'){
					type = 1; //遇到第二个'>'，追加重定向
					start++;
				}
				break;
			}
			if (*start == '<'){
				type = 2; //遇到'<'，输入重定向
				*start = '\0';
				start++;
				break;
			}
			start++;
		}
		if (*start != '\0'){ //start位置不为'\0'，说明命令包含重定向内容
			while (isspace(*start)) //跳过重定向符号后面的空格
				start++;
		}
		else{
			start = NULL; //start设置为NULL，标识命令当中不含重定向内容
		}

		//拆分命令
		myargv[0] = strtok(cmd, " ");
		int i = 1;
		while (myargv[i] = strtok(NULL, " ")){
			i++;
		}
		pid_t id = fork(); //创建子进程执行命令
		if (id == 0){
			//child
			if (start != NULL){
				if (type == 0){ //输出重定向
					int fd = open(start, O_WRONLY | O_CREAT | O_TRUNC, 0664); //以写的方式打开文件（清空原文件内容）
					if (fd < 0){
						error("open");
						exit(2);
					}
					close(1);
					dup2(fd, 1); //重定向
				}
				else if (type == 1){ //追加重定向
					int fd = open(start, O_WRONLY | O_APPEND | O_CREAT, 0664); //以追加的方式打开文件
					if (fd < 0){
						perror("open");
						exit(2);
					}
					close(1);
					dup2(fd, 1); //重定向
				}
				else{ //输入重定向
					int fd = open(start, O_RDONLY); //以读的方式打开文件
					if (fd < 0){
						perror("open");
						exit(2);
					}
					close(0);
					dup2(fd, 0); //重定向
				}
			}

			execvp(myargv[0], myargv); //child进行程序替换
			exit(1); //替换失败的退出码设置为1
		}
		//shell
		int status = 0;
		pid_t ret = waitpid(id, &status, 0); //shell等待child退出
		if (ret > 0){
			printf("exit code:%d\n", WEXITSTATUS(status)); //打印child的退出码
		}
	}
	return 0;
}

