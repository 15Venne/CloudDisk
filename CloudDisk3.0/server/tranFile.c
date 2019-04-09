#include"head.h"

int tranFile(char *path, char *filename, int new_fd)
{
    train data;
    memset(&data, 0, sizeof(data));
    int ret;
    char md5sum[33]={0};
    char truePath[100]={0};
    sprintf(truePath, "/home/venne/CloudDisk/usr/%s/%s", path, filename);
    //printf("%s\n", truePath);
    int fd = open(truePath, O_RDONLY);
    if(0 == strlen(filename))
    {
        fd = -1;
    }
    send(new_fd, (char*)&fd, 4, 0); //告诉客户端此文件存不存在
    //printf("fd=%d\n", fd);
    if(-1 == fd)
    {
        return -1;
    }
    getMD5(md5sum, fd);
    send(new_fd, md5sum, 32, 0); //发送文件md5值

    struct stat filebuf;
    fstat(fd, &filebuf);
    data.dataLen = sizeof(filebuf.st_size);
    memcpy(data.buf, &filebuf.st_size, sizeof(filebuf.st_size));

    send(new_fd, (char*)&data, data.dataLen+4, 0); //发送文件大小

    recv(new_fd, (char*)&data.dataLen, 4, 0);//接收偏移量
    //printf("收到偏移量=%d\n", data.dataLen);
    //如果之前有中断，偏移量为之前发送的大小
    //如果没有，则会收到0
    if(filebuf.st_size-data.dataLen > 0)
    {
        char *p=mmap(NULL, filebuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if((char*)-1==p)
        {
            perror("mmap");
            printf("mmap failed\n");
            return -1;
        }
        ret = sendPlus(new_fd, p+data.dataLen, filebuf.st_size-data.dataLen);
        if(-1 == ret)
        {
            return -1;
        }
        munmap(p, filebuf.st_size-data.dataLen);
    }
    data.dataLen=0;
    send(new_fd, (char*)&data.dataLen, 4, 0);
    close(fd);
    return 0;

}
