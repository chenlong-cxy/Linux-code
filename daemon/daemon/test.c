//�����ػ�����
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	//1�������ļ�����Ϊ0
	umask(0);

	//2��fork����ֹ�����̣��ӽ��̴����»Ự
	if (fork() > 0){
		//father
		exit(0);
	}
	setsid();

	//3������SIGCHLD�ź�
	signal(SIGCHLD, SIG_IGN);

	//4���ٴ�fork����ֹ�����̣������ӽ��̲��ǻỰ�׽��̣��Ӷ���֤���������ٺ������ն������
	//�����Ǳ���ģ������Ա�̣�
	if (fork() > 0){
	    //father
		exit(0);
	}

	//5�����Ĺ���Ŀ¼Ϊ��Ŀ¼����ѡ��ѡ�
	chdir("/");

	//6������׼���롢��׼�������׼�����ض���/dev/null����ѡ��ѡ�
	close(0);
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, 1);
	dup2(fd, 2);
	
	while (1);
	return 0;
}

//ʹ�ýӿڴ����ػ�����
#include <unistd.h>

int main()
{
	daemon(0, 0);
	while (1);
	return 0;
}

//ģ��ʵ��daemon�ӿ�
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

void my_daemon(int nochdir, int noclose)
{
	//1�������ļ�����Ϊ0
	umask(0);

	//2��fork����ֹ�����̣��ӽ��̴����»Ự
	if (fork() > 0){
		//father
		exit(0);
	}
	setsid();

	//3������SIGCHLD�ź�
	signal(SIGCHLD, SIG_IGN);

	//4���ٴ�fork����ֹ�����̣������ӽ��̲��ǻỰ�׽��̣��Ӷ���֤���������ٺ������ն������
	//�����Ǳ���ģ������Ա�̣�
	if (fork() > 0){
		//father
		exit(0);
	}

	//5�����Ĺ���Ŀ¼Ϊ��Ŀ¼����ѡ��ѡ�
	if (nochdir == 0){
		chdir("/");
	}

	//6������׼���롢��׼�������׼�����ض���/dev/null����ѡ��ѡ�
	if (noclose == 0){
		close(0);
		int fd = open("/dev/null", O_RDWR);
		dup2(fd, 1);
		dup2(fd, 2);
	}
}
int main()
{
	my_daemon(0, 0);
	while (1);
	return 0;
}
