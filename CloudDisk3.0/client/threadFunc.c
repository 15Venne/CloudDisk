#include"head.h"

void *threadFuncGetsPuts(void *p)
{
    tokenNode *pt=(tokenNode*)p;
    int PutsGetsFd;
    int ret;
    ret=tcpConnect(&PutsGetsFd, pt->ip, pt->port);
    if(-1==ret)
    {
        perror("tcpConnect");
    }
    //printf("PutsGetsFd=%d\n", PutsGetsFd);
    //printf("子线程已连上服务器\n");
    send(PutsGetsFd, pt->token, 9, 0); //协议，连接成功发送token，申请认证
    recv(PutsGetsFd, (char*)&ret, 4, 0);//接收认证结果
    if(ret!=1)
    {
        printf("子线程认证失败,下载上传功能失效\n");
        pthread_exit(NULL);
    }
    char filename[50]={0};
    //char pathC[100]={0};
    while(1)
    {
        pthread_mutex_lock(&pt->mutex);
        //printf("子线程在wait\n");
        pthread_cond_wait(&pt->cond, &pt->mutex);
        //printf("子线程被唤醒\n");
        send(PutsGetsFd, (char*)&pt->orderNum, 4, 0);//发送指令号码
        ret = strlen(pt->buf);                        //即命令前半截       
        send(PutsGetsFd, (char*)&ret, 4, 0);//发送buf长度
        send(PutsGetsFd, pt->buf, ret, 0); //发送命令后半截
        ret = strlen(pt->path);
        send(PutsGetsFd, (char*)&ret, 4, 0);
        send(PutsGetsFd, pt->path, ret, 0); //发送操作的路径

        memset(filename, 0, sizeof(filename));
        //memset(pathC, 0, sizeof(path));
        strcpy(filename, pt->buf);
        //strcpy(pathC, pt->path);
        pthread_mutex_unlock(&pt->mutex);
        
        if(PUTS_CMD == pt->orderNum)
        {
            printf("申请上传\n");
            ret = putsrecv(PutsGetsFd, filename);
        }
        else if(GETS_CMD == pt->orderNum)
        {
            printf("申请下载\n");
            ret = getsrecv(PutsGetsFd, filename);
        }
    }
}
