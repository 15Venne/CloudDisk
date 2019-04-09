#include"head.h"
int is_sameFile_exist(char *filename, off_t *fileSizeLocal)
{
    struct dirent *p;
    int ret;
    struct stat cur;
    char buf[300]={0};
    char *path=getcwd(NULL, 0);
    DIR *dir=opendir(path);
    if(NULL==dir)
    {
        return -1;
    }
    while((p=readdir(dir))!=NULL)
    {
        if('.' == p->d_name[0])
        {
            continue;
        }
        if(strcmp(p->d_name, filename)!=0)
        {
            continue;
        }
        sprintf(buf, "%s/%s", path, p->d_name);
        ret=stat(buf, &cur);
        if(-1==ret) {return -1;}
        *fileSizeLocal=cur.st_size;
        return 1;
    }
    return 0;
}

int getsrecv(int socketFd, char *filename)
{
    int dataLen, ret, fd;
    char buf[200]={0};
    char tmpbuf[200]={0};
    char md5sum[33]={0};
    char md5sumLocal[33]={0};
    //recv(socketFd, (char*)&dataLen, 4, 0);
    //recv(socketFd, buf, dataLen, 0); //文件名
    recv(socketFd, (char*)&dataLen, 4, 0); //接收服务器文件真假信息
    if(-1 == dataLen)
    {
        printf("file doesn't exist\n");
        return -1;
    }
    recv(socketFd, md5sum, 32, 0); //接收文件MD5
    off_t fileSize;
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, (char*)&fileSize, dataLen, 0); //接收文件大小

    off_t fileSizeLocal=0;
    ret = is_sameFile_exist(filename, &fileSizeLocal);//判断有无同名文件
    if(-1==ret){ return -1;}
    sprintf(tmpbuf, "%s%s", filename, ".tmp"); 
    int tmpFd = open(tmpbuf, O_CREAT|O_RDWR, 0666);  //打开或创建临时文件
    if(-1==tmpFd){ return -1;}

    if(0==ret) //本地无同名文件，直接下载
    {
        write(tmpFd, md5sum, 32);
        fd = open(filename, O_CREAT|O_RDWR, 0666);
        if(-1==fd) {return -1;}
    }
    else if(1==ret) //本地有同名文件
    {
        read(tmpFd, md5sumLocal, 32);
        if(md5sumLocal[0]=='\0') //如果本地同名文件的md5为0，则为完整文件
        {                         //计算其md5
            fd=open(filename, O_RDONLY);
            if(-1==fd) {return -1;}
            getMD5(md5sumLocal, fd);
            close(fd);
        }
        if(strcmp(md5sum, md5sumLocal)!=0) //如果与旧md5不同，则为不同文件
        {                    
            //printf("md5不同\n");            //则删除原文件，覆盖旧md5
            unlink(filename);
            lseek(tmpFd, -32, SEEK_CUR);
            write(tmpFd, md5sum, 32);
            fd = open(filename, O_CREAT| O_RDWR, 0666);
            if(-1==fd) {return -1;}
            fileSizeLocal = 0;
            ret=0; //ret置成0，为了下面ftruncate条件
        }
        else       //与之前的一样，说明上次下载中断
        {
            //printf("md5相同\n");
            fd=open(filename, O_CREAT|O_RDWR, 0666);
            if(-1==fd) {return -1;}
            read(tmpFd, (char*)&fileSizeLocal, 8);
            //printf("取出的本地大小=%ld\n", fileSizeLocal);
            lseek(fd, fileSizeLocal, SEEK_SET); //偏移到中断位置
            //fileSize=fileSize-fileSizeLocal;
        }
    }
    send(socketFd, (char*)&fileSizeLocal, 4, 0); //发送偏移量给服务器
    //printf("偏移量已发送\n");
    //只发送4个字节存在隐患
    //以下接收文件内容
    if(0==ret)
    {
        ftruncate(fd, fileSize);
    }

    if(fileSize-fileSizeLocal > 0)
    {
        char *p = mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if((char*)-1==p)
        {
            printf("mmap failed\n");
            return -1;
        }
        ret = recvPlus(socketFd, p+fileSizeLocal, fileSize-fileSizeLocal, &fileSizeLocal, tmpFd);
        if(-1==ret)
        {
            printf("lose connection...\n");
            close(fd);
            close(tmpFd);
            return -1;
        }
        munmap(p,fileSize);
    }
    else
    {
        printf("file already exists\n");
    }
    //printf("mmap解除\n");
    recv(socketFd, (char*)&dataLen, 4, 0);
    printf("download completed\n");
    close(fd);
    close(tmpFd);
    unlink(tmpbuf);
    return 0;
}
