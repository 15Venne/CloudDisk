#include"head.h"

int sendPlus(int fd, char *p, int lenth)
{
    int total=0;
    int ret;
    while(total < lenth)
    {
        ret = send(fd, p+total, lenth-total, 0);
        if(-1 == ret)
        {
            return -1;
        }
        total +=ret;
    }
    return 0;
}

int recvPlus(int fd, char *p, int lenth)
{
    int total=0;
    int ret;
    while(total<lenth)
    {
        ret = recv(fd, p+total, lenth-total, 0);
        if(0 == ret)
        {
            return -1;
        }
        total = total+ret;
    }
    return 0;
}

int mypwd(int new_fd)
{
    char *chpwd=getcwd(NULL, 0);
    int dataLen;
    //int ret;
    dataLen=strlen(chpwd);
    sendPlus(new_fd, (char*)&dataLen, 4);
    sendPlus(new_fd, chpwd, dataLen);
    dataLen = 0;
    sendPlus(new_fd, (char*)&dataLen, 4);
    return 0;
}

int mycd(char *pdir, int new_fd, char *usr_name)
{
    int ret;
    char *oripwd = getcwd(NULL, 0); 
    chdir(pdir);
    char *chpwd = getcwd(NULL, 0);
    ret=0;
    while(chpwd[ret]!='r')
    {
        ret++;
    }
    chpwd=chpwd+ret+2;
    ret=0;
    while(chpwd[ret]!='\0' && chpwd[ret]!='/')
    {
        ret++;
    }
    chpwd[ret]='\0';
    ret = strcmp(chpwd, usr_name);
    send(new_fd, (char*)&ret, 4, 0);
    if(ret!=0)
    {
        chdir(oripwd);
        return -1;
    }
    
    return 0;
}

int myremove(char *p, int new_fd)
{
    int ret;
    int dataLen;
    ret = unlink(p);
    if(-1==ret)
    {
        dataLen=13;
        send(new_fd, (char*)&dataLen, 4, 0);
        send(new_fd, "remove failed", 13, 0);
        return -1;
    }
    else if(0==ret)
    {
        dataLen=16;
        send(new_fd, (char*)&dataLen, 4, 0);
        send(new_fd, "remove completed", 16, 0);
    }
    return 0;
}

int mymkdir(char *p, int new_fd)
{
    int ret;
    ret = mkdir(p, 0775);
    int dataLen;
    if(-1==ret)
    {
        dataLen=12;
        send(new_fd, (char*)&dataLen, 4, 0);
        send(new_fd, "mkdir failed", 12, 0);
        return -1;
    }
    else if(0==ret)
    {
        dataLen=15;
        send(new_fd, (char*)&dataLen, 4, 0);
        send(new_fd, "mkdir completed", 15, 0);
    }
    return 0;
}
