//���ض��ڴ�����������
void bzero(void *s, size_t n);

//����
int listen(int sockfd, int backlog);

//��ȡ����
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

//���ӷ�����
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);