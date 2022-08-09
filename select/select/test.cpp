int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);      //用来清除描述词组set中相关fd的位
int  FD_ISSET(int fd, fd_set *set);    //用来测试描述词组set中相关fd的位是否为真
void FD_SET(int fd, fd_set *set);      //用来设置描述词组set中相关fd的位
void FD_ZERO(fd_set *set);             //用来清除描述词组set的全部位


fd_set readset;
FD_SET(fd, &readset);
select(fd + 1, &readset, NULL, NULL, NULL);
if (FD_ISSET(fd, readset)){...}
