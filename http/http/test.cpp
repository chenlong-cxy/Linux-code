#include <iostream>
using namespace std;
class Time
{
public:
	Time(int hour = 0)
	{
		cout << "Time" << endl;
		_hour = hour;
	}
private:
	int _hour;
};
class Test
{
public:
	// �ڹ��캯�����ڳ�ʼ������ʹ�ó�ʼ���б�
	Test(int hour)
	{
		Time t(hour);// ����һ��Time��Ĺ��캯��
		_t = t;// ����һ��Time��ĸ�ֵ��������غ���
	}
private:
	Time _t;
};
int main()
{
	Test(12);
	return 0;
}



#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
	//�����׽���
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0){
		cerr << "socket error!" << endl;
		return 1;
	}
	//��
	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(8081);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		cerr << "bind error!" << endl;
		return 2;
	}
	//����
	if (listen(listen_sock, 5) < 0){
		cerr << "listen error!" << endl;
		return 3;
	}
	//����������
	struct sockaddr peer;
	memset(&peer, 0, sizeof(peer));
	socklen_t len = sizeof(peer);
	for (;;){
		int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if (sock < 0){
			cerr << "accept error!" << endl;
			continue;
		}
		if (fork() == 0){ //�ְֽ���
			close(listen_sock);
			if (fork() > 0){ //�ְֽ���
				exit(0);
			}
			//���ӽ���
			char buffer[1024];
			recv(sock, buffer, sizeof(buffer), 0); //��ȡHTTP����
			cout << "--------------------------http request begin--------------------------" << endl;
			cout << buffer << endl;
			cout << "---------------------------http request end---------------------------" << endl;
			
#define PAGE "index.html" //��վ��ҳ
			//��ȡindex.html�ļ�
			ifstream in(PAGE);
			if (in.is_open()){
				in.seekg(0, in.end);
				int len = in.tellg();
				in.seekg(0, in.beg);
				char* file = new char[len];
				in.read(file, len);
				in.close();
				
				//����HTTP��Ӧ
				string status_line = "http/1.1 200 OK\n"; //״̬��
				string response_header = "Content-Length: " + to_string(len) + "\n"; //��Ӧ��ͷ
				string blank = "\n"; //����
				string response_text = file; //��Ӧ����
				string response = status_line + response_header + blank + response_text; //��Ӧ����
				
				//��ӦHTTP����
				send(sock, response.c_str(), response.size(), 0);

				delete[] file;
			}
			close(sock);
			exit(0);
		}
		//үү����
		close(sock);
		waitpid(-1, nullptr, 0); //�ȴ��ְֽ���
	}
	return 0;
}


#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
	//�����׽���
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0){
		cerr << "socket error!" << endl;
		return 1;
	}
	//��
	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(8081);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		cerr << "bind error!" << endl;
		return 2;
	}
	//����
	if (listen(listen_sock, 5) < 0){
		cerr << "listen error!" << endl;
		return 3;
	}
	//����������
	struct sockaddr peer;
	memset(&peer, 0, sizeof(peer));
	socklen_t len = sizeof(peer);
	for (;;){
		int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if (sock < 0){
			cerr << "accept error!" << endl;
			continue;
		}
		if (fork() == 0){ //�ְֽ���
			close(listen_sock);
			if (fork() > 0){ //�ְֽ���
				exit(0);
			}
			//���ӽ���
			char buffer[1024];
			recv(sock, buffer, sizeof(buffer), 0); //��ȡHTTP����
			cout << "--------------------------http request begin--------------------------" << endl;
			cout << buffer << endl;
			cout << "---------------------------http request end---------------------------" << endl;
			
			//����HTTP��Ӧ
			string status_line = "http/1.1 307 Temporary Redirect\n"; //״̬��
			string response_header = "Location: https://www.csdn.net/\n"; //��Ӧ��ͷ
			string blank = "\n"; //����
			string response = status_line + response_header + blank; //��Ӧ����
			
			//��ӦHTTP����
			send(sock, response.c_str(), response.size(), 0);

			close(sock);
			exit(0);
		}
		//үү����
		close(sock);
		waitpid(-1, nullptr, 0); //�ȴ��ְֽ���
	}
	return 0;
}


#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
	//�����׽���
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0){
		cerr << "socket error!" << endl;
		return 1;
	}
	//��
	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(8081);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		cerr << "bind error!" << endl;
		return 2;
	}
	//����
	if (listen(listen_sock, 5) < 0){
		cerr << "listen error!" << endl;
		return 3;
	}
	//����������
	struct sockaddr peer;
	memset(&peer, 0, sizeof(peer));
	socklen_t len = sizeof(peer);
	for (;;){
		int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if (sock < 0){
			cerr << "accept error!" << endl;
			continue;
		}
		if (fork() == 0){ //�ְֽ���
			close(listen_sock);
			if (fork() > 0){ //�ְֽ���
				exit(0);
			}
			//���ӽ���
			char buffer[1024];
			recv(sock, buffer, sizeof(buffer), 0); //��ȡHTTP����
			cout << "--------------------------http request begin--------------------------" << endl;
			cout << buffer << endl;
			cout << "---------------------------http request end---------------------------" << endl;

#define PAGE "index.html" //��վ��ҳ
			//��ȡindex.html�ļ�
			ifstream in(PAGE);
			if (in.is_open()){
				in.seekg(0, in.end);
				int len = in.tellg();
				in.seekg(0, in.beg);
				char* file = new char[len];
				in.read(file, len);
				in.close();

				//����HTTP��Ӧ
				string status_line = "http/1.1 200 OK\n"; //״̬��
				string response_header = "Content-Length: " + to_string(len) + "\n"; //��Ӧ��ͷ
				response_header += "Set-Cookie: 2021dragon\n"; //���Set-Cookie�ֶ�
				string blank = "\n"; //����
				string response_text = file; //��Ӧ����
				string response = status_line + response_header + blank + response_text; //��Ӧ����
				
				//��ӦHTTP����
				send(sock, response.c_str(), response.size(), 0);

				delete[] file;
			}
			close(sock);
			exit(0);
		}
		//үү����
		close(sock);
		waitpid(-1, nullptr, 0); //�ȴ��ְֽ���
	}
	return 0;
}