//对特定内存区域进行清空
void bzero(void *s, size_t n);

//监听
int listen(int sockfd, int backlog);

//获取连接
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
