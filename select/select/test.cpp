int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);      //���������������set�����fd��λ
int  FD_ISSET(int fd, fd_set *set);    //����������������set�����fd��λ�Ƿ�Ϊ��
void FD_SET(int fd, fd_set *set);      //����������������set�����fd��λ
void FD_ZERO(fd_set *set);             //���������������set��ȫ��λ


fd_set readset;
FD_SET(fd, &readset);
select(fd + 1, &readset, NULL, NULL, NULL);
if (FD_ISSET(fd, readset)){...}
