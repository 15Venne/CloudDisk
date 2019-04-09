#include"head.h"


int tcpConnect(int *sfd, const char *ip, int port)
{
    int ret;
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    *sfd = socket(AF_INET, SOCK_STREAM, 0);
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip);
    ret = connect(*sfd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    return ret;
}

