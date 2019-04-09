#include"head.h"


int tcpInit(int *sfd, char *ip, int port)
{
    *sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == *sfd)
    {
        perror("socket");
    }
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip);

    int ret, reuse=1;
    ret = setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    if(-1 == ret)
    {
        perror("setsockopt");
    }
    ret = bind(*sfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
    }

    listen(*sfd, 10);
    return 0;
}
