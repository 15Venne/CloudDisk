#include"head.h"

int sendPlus(int fd, char *p, int lenth)
{
    int total=0;
    int ret;
    /* int pretotal=0;//上一次的大小 */
    /* int fileSizepi = lenth/100; */
    printf("\n上传中，上传量=%d\n", lenth);
    /* double temp; */
    while(total < lenth)
    {
        ret = send(fd, p+total, lenth-total, 0);
        if(-1 == ret)
        {
            return -1;
        }
        total +=ret;
        /* if(total - pretotal >= fileSizepi) */
        /* { */
        /*     temp = (double)total/lenth*100; */
            /* printf("\r"); */
            /* printf("\033[42;32m%*s\033[0m", (int)temp/2, " "); */
            /* printf("%*s", (50-(int)temp/2), " "); */
            /* printf("|%4.2f%s", temp, "%"); */
            /* fflush(stdout); */
            /* pretotal = total; */
        /* } */
    }
    /* printf("\r\033[42;32m%*s\033[0m|100.00%s\n", 51, " ", "%"); */
    return 0;
}

int recvPlus(int fd, char *p, int lenth, off_t *fileSizeLocal, int tmpFd)
{
    int total=0;//接收总大小
    int ret;
    /* int pretotal=0;//上一次的大小 */
    /* int fileSizepi = lenth/100; */
    /* double temp; */
    int j=1024*1024;
    while(total<lenth)
    {
        ret = recv(fd, p+total, lenth-total, 0);
        if(0 == ret)
        {
            return -1;
        }
        total = total+ret;
        *fileSizeLocal = *fileSizeLocal + ret;
        //printf("fileSizeLocal=%ld\n", *fileSizeLocal);
        if(*fileSizeLocal  > j)
        {
            lseek(tmpFd, 32, SEEK_SET);
            write(tmpFd, (char*)fileSizeLocal, 8);
            j=j+5*1024*1024;
        }
        /* if(total - pretotal >= fileSizepi) */
        /* { */
        /*     temp = (double)total/lenth*100; */
        /*     printf("\r"); */
        /*     printf("\033[42;32m%*s\033[0m", (int)temp/2, " "); */
        /*     printf("%*s", (50-(int)temp/2), " "); */
        /*     printf("|%4.2f%s", temp, "%"); */
        /*     fflush(stdout); */
        /*     pretotal = total; */
        /* } */
    }
    /* printf("\r\033[42;32m%*s\033[0m|100.00%s\n", 51, " ", "%"); */
    return 0;
}

int orderCheck(char *p, char *p1)
{
    char *q=(char*)malloc(sizeof(strlen(p)));
    strcpy(q,p);
    int i=0;
    while(q[i]!=' '  && q[i]!='\n')
    {
        i++;
    }
    q[i]='\0';
    strcpy(p1, p+i+1);
    i=0;
    while(p1[i]!='\n'&&p1[i]!='\0')
    {
        i++;
    }
    p1[i]='\0';
    //if(0==strcmp(q, "cd")||0==strcmp(q, "ls")||0==strcmp(q, "puts")
    //   ||0==strcmp(q, "gets")||0==strcmp(q, "remove")||0==strcmp(q, "pwd")||0==strcmp(q, "mkdir")){}
    
    if(0==strcmp(q, "ls"))
    {
        free(q);
        return LS_CMD;
    }
    else if(0==strcmp(q, "cd"))
    {
        free(q);
        return CD_CMD;
    }
    else if(0==strcmp(q, "puts"))
    {
        free(q);
        return PUTS_CMD; 
    }
    else if(0==strcmp(q, "gets"))
    {
        free(q);
        return GETS_CMD;
    }
    else if(0==strcmp(q, "remove"))
    {
        free(q);
        return REMOVE_CMD;
    }
    else if(0==strcmp(q, "pwd"))
    {
        free(q);
        return PWD_CMD;
    }
    else if(0==strcmp(q, "mkdir"))
    {
        free(q);
        return MKDIR_CMD;
    }
    else return -1;
}

int lsrecv(int socketFd)
{
    char buf[1000]={0};
    int dataLen;
    int ret;
    recv(socketFd, (char*)&ret, 4, 0);
    if(ret!=0)
    {
        printf("没有上一层了!\n");
        return -1;
    }
    while(1)
    {
        ret = recv(socketFd, (char*)&dataLen, 4, 0);
        if(-1==ret) {return -1;}
        if(dataLen>0)
        {
            memset(buf, 0, sizeof(buf));
            ret = recv(socketFd, buf, dataLen, 0);
            if(-1==ret){return -1;}
            if(buf[0]=='d')
            {
                printf("\e[0;34m%s\033[0m\n", buf);
            }
            else
                printf("%s\n", buf);
        }
        else {break;}
    }
    return 0;
}

int pwdrecv(int socketFd)
{
    char buf[500];
    int dataLen;
    memset(buf, 0, sizeof(buf));
    //int ret;
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, buf, dataLen, 0);
    printf("%s\n", buf);
    recv(socketFd, (char*)&dataLen, 4, 0);
    return 0;
}
int cdrecv(int socketFd)
{
    int ret;
    recv(socketFd, (char*)&ret, 4, 0);
    if(ret!=0)
    {
        printf("没有上一层了\n");
        return -1;
    }
    return 0;
}



int removerecv(int socketFd)
{
    char buf[30];
    int dataLen;
    memset(buf, 0, sizeof(buf));
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, buf, dataLen, 0);
    printf("%s\n",buf);
    return 0;
}


int mkdirrecv(int socketFd)
{
    char buf[30];
    int dataLen;
    memset(buf, 0, sizeof(buf));
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, buf, dataLen, 0);
    printf("%s\n", buf);
    return 0;
}
