#include"head.h"

int is_sameFile_exist(char *truePath, char *filename, off_t *fileSizeLocal)
{
    struct dirent *p;
    int ret;
    struct stat cur;
    char buf[300]={0};
    //char *path=getcwd(NULL, 0);
    DIR *dir=opendir(truePath);
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
        sprintf(buf, "%s/%s", truePath, p->d_name);
        ret=stat(buf, &cur);
        if(-1==ret) {return -1;}
        *fileSizeLocal=cur.st_size;
        closedir(dir);
        return 1;
    }
    closedir(dir);
    return 0;
}

int recvFile(char *path, char *filename, int new_fd)
{
    int dataLen;
    char buf[200]={0};
    char tmpbuf[200]={0};
    char truePath[200]={0};
    int ret;
    char md5sum[33]={0};
    char md5sumLocal[33]={0};
    recv(new_fd, (char*)&ret, 4, 0);//客户端通知
    if(ret == -1)
    {
        return -1;
    }
    recvPlus(new_fd, (char*)&dataLen, 4);
    recvPlus(new_fd, buf, dataLen); //接收文件名
    printf("接收到的文件名:%s\n", buf);
    //printf("filename=%s\n", buf);
    off_t fileSize;
    off_t fileSizeLocal=0;
    recvPlus(new_fd, (char*)&dataLen, 4);
    recvPlus(new_fd, (char*)&fileSize, dataLen);//接收文件大小
    //printf("fileSize=%ld\n",fileSize);
    recv(new_fd, (char*)&dataLen, 4, 0);
    recv(new_fd, md5sum, dataLen, 0);  //接收文件md5码
    //printf("接收md5成功，md5=%s\n", md5sum);
    sprintf(truePath, "/home/venne/CloudDisk/usr/%s", path);
    sprintf(buf, "%s/%s", truePath, filename);//拼接文件名
    //printf("buf:%s\n", buf);
    ret = is_sameFile_exist(truePath, filename, &fileSizeLocal);//判断有无同名文件,如果有,fileSizeLocal返回同名文件大小
    if(-1==ret) {return -1;}
    //printf("判断有无同名成功，ret=%d\n", ret);
    sprintf(tmpbuf, "%s%s", buf, ".tmp");
    //printf("tmpbuf:%s\n", tmpbuf);
    int fd;
    int tmpFd = open(tmpbuf, O_CREAT|O_RDWR, 0666); //生成或打开临时文件
    if(-1==tmpFd) {return -1;}

    if(0==ret) //无同名文件,直接下载
    {
        write(tmpFd, md5sum, 32);
        fd = open(buf, O_CREAT|O_RDWR, 0666);
        if(-1==fd) { close(tmpFd); return -1;}
    }
    else if(1==ret)//有同名文件
    {
        read(tmpFd, md5sumLocal, 32);
        if(md5sumLocal[0]=='\0') //如果md5local为0，则本地无配置文件,则说明同名文件为完整文件,取完整文件的md5
        {                                     
            fd=open(buf, O_RDONLY);
            getMD5(md5sumLocal, fd);    //计算文件md5时间较长，改用数据库存储文件可减少服务器压力
            close(fd);
        }
        // printf("文件同名,md5sumLocal=%s\n", md5sumLocal);
        //printf("md5sumlen=%ld, md5sumLocalsum=%ld\n", strlen(md5sum), strlen(md5sumLocal));
        if(strcmp(md5sum, md5sumLocal)!=0) //与之前的md5码不一样,删掉原文件,覆盖旧md5码
        {
            unlink(buf);
            // printf("删除同名文件成功\n");
            lseek(tmpFd, -32, SEEK_CUR);
            write(tmpFd, md5sum, 32);
            fd = open(buf, O_CREAT|O_RDWR, 0666);
            if(-1==fd) { close(tmpFd); return -1;}
            fileSizeLocal=0;
        }
        else //与之前md5码一样，说明是上次上传中断
        {
            fd = open(buf, O_CREAT|O_RDWR, 0666);
            if(-1==fd) { close(tmpFd); return -1;}
            lseek(fd, fileSizeLocal, SEEK_SET); //偏移到中断的位置
            fileSize = fileSize - fileSizeLocal;
            //printf("发现中断文件, 偏移成功\n");
        }

    }
    send(new_fd, (char*)&fileSizeLocal, 4, 0); //发送偏移量给客户端
                                        //fileSizeLocal为off_t,只发送4个字节存在隐患
    //以下获取文件路径
    /* char *pwd=getcwd(NULL, 0); */
    /* ret=0; */
    /* while(pwd[ret]!='r') */
    /* { */
    /*     ret++; */
    /* } */
    /* pwd=pwd+ret+2; */
    //插入数据库
    //ret = insertTable_fileHouse(filename, pwd, md5sum, fileSize);
    /* if(-1==ret) */
    /* { */
    /*     printf("文件数据创建失败\n"); */
    /*     close(fd); */
    /*     return -1; */
    /* } */
    //ftruncate(fd, fileSize);
    //char *p=mmap(NULL, fileSize,  PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    //ret =recvPlus(new_fd, p, fileSize);         //接收文件内容
    //if(-1==ret)
    //{
    //    return -1;
    //}
    int fds[2];
    pipe(fds);
    int recvcnt;
    if(fileSize > 0)
    {
        while(fileSize > 65535) //接收文件内容
        {
            recvcnt = splice(new_fd, NULL, fds[1], NULL, 65535, SPLICE_F_MORE | SPLICE_F_MOVE);
            if(recvcnt == 0)
            {
                close(fd); close(tmpFd); close(fds[1]); close(fds[0]);
                return -1;
            }
            if(-1 == recvcnt)
            {
                printf("while1splice1 error\n");
                close(fd); close(tmpFd); close(fds[1]); close(fds[0]);
                return -1;
            }
            ret = splice(fds[0], NULL, fd, NULL, recvcnt, SPLICE_F_MORE | SPLICE_F_MOVE);
            if(-1 == ret)
            {
                printf("while2splice2 error\n"); 
                close(fd); close(tmpFd); close(fds[1]); close(fds[0]);
                return -1;
            }
            fileSize -= recvcnt;
        }
        /* printf("last=%ld\n",fileSize); */
        recvcnt = splice(new_fd, NULL, fds[1], NULL, fileSize, SPLICE_F_MORE | SPLICE_F_MOVE);

        ret = splice(fds[0], NULL, fd, NULL, recvcnt, SPLICE_F_MORE | SPLICE_F_MOVE);

        fileSize = fileSize - recvcnt;
        if(fileSize!=0)
        {
            recvcnt = splice(new_fd, NULL, fds[1], NULL, fileSize, SPLICE_F_MORE | SPLICE_F_MOVE);

            ret = splice(fds[0], NULL, fd, NULL, recvcnt, SPLICE_F_MORE | SPLICE_F_MOVE);  
        }
    }
    recvPlus(new_fd, (char*)&dataLen, 4); //结束符
    close(tmpFd);
    close(fd);
    close(fds[1]); close(fds[0]);
    unlink(tmpbuf);
    return 0;
}
