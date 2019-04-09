#include"head.h"


int putsrecv(int socketFd, char *filename)
{
    train data;
    memset(&data, 0, sizeof(data));
    int ret, fd;
    char md5sum[33]={0};
    fd = open(filename, O_RDONLY);
    DIR *dir=opendir(filename);
    if(dir!=NULL)
    {
        fd=-1;
    }
    closedir(dir);
    send(socketFd, (char*)&fd, 4, 0);//告诉客户端文件是否存在
    if(-1 == fd)
    {
        printf("file doesn't exist\n");
        return -1;
    }
    
    data.dataLen=strlen(filename);
    strcpy(data.buf, filename);
    send(socketFd, (char*)&data, data.dataLen+4, 0);//发送文件名

    struct stat filebuf;
    fstat(fd, &filebuf);
    data.dataLen = sizeof(filebuf.st_size);
    memcpy(data.buf, &filebuf.st_size, sizeof(filebuf.st_size));
    //printf("fileSize=%ld\n", filebuf.st_size);
    send(socketFd, (char*)&data, data.dataLen+4, 0); //发送文件大小
    
    memset(&data, 0, sizeof(data));
    getMD5(data.buf, fd);
    data.dataLen=strlen(data.buf);
    send(socketFd, (char*)&data, data.dataLen+4, 0);//发送文件md5码

    char *p=mmap(NULL, filebuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if((char*)-1==p)
    {
        printf("mmap failed\n");
        return -1;
    }

    recv(socketFd, (char*)&data.dataLen, 4, 0); //接收偏移量
                                        //如果之前有中断，则是之前上传的大小
                                        //如果没有，则收到0;
    ret =sendPlus(socketFd, p+data.dataLen, filebuf.st_size-data.dataLen); //发送文件内容
    if(-1==ret)
    {
        return -1;
    }

    data.dataLen = 0;
    send(socketFd, (char*)&data, 4, 0);  //结束符
    printf("upload completed\n");
    close(fd);
    munmap(p, filebuf.st_size);
    return 0;
   
}
