#include"head.h"

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    int ret, i;
    int socketFd;
    tcpInit(&socketFd, argv[1], myport);

    int epfd = epoll_create(1);
    struct epoll_event event, *evs;
    evs = (struct epoll_event*)calloc(childNum+1, sizeof(struct epoll_event));

    //以下注册socketFd
    event.events = EPOLLIN;
    event.data.fd = socketFd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, socketFd, &event);
    if(-1==ret) { perror("epoll_ctl"); }

    ProcessData *p=(ProcessData*)calloc(childNum, sizeof(ProcessData));
    makeChild(p, childNum); //建子进程
    //以下注册子进程fd
    for(i=0;i<childNum;++i)
    {
        event.events = EPOLLIN;
        event.data.fd = p[i].fd;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, p[i].fd, &event);
        if(-1 == ret)
        {
            perror("epoll_ctl");
        }
    }
    int j, new_fd, readyNum;
    char buf[30]={0};
    char token[9]={0};
    //以下建子线程，所有puts和gets任务交给此子线程完成
    
    pthread_t pthreadChild;
    threadNode tNode;
    memset(&tNode, 0, sizeof(tNode));
    tNode.fdMax=10;
    tNode.new_fd=(int*)calloc(10, sizeof(int));
    pipe(tNode.fds);
    pthread_create(&pthreadChild, NULL, threadFuncChild, &tNode);//启动子线程

    while(1)
    {
        readyNum = epoll_wait(epfd, evs, childNum+1, -1);
        if(-1 == readyNum) { perror("epoll_wait"); }
        for(i=0;i<readyNum;++i)  //遍历evs[]
        {
            if(evs[i].data.fd == socketFd) //如果有客户端连接
            {
                new_fd = accept(socketFd, NULL, NULL);
                printf("client connect success, new_fd =%d\n", new_fd);
                recv(new_fd, token, 9, 0);//接收token
                ret = checkToken(token);//认证       //该认证最好带用户名，但此时客户端还没有传用户名，待改良
                send(new_fd, (char*)&ret, 4, 0);//返回认证结果
                if(0 == ret) //如果不存在此token,则说明是登陆用户
                {
                    for(j=0;j<childNum;++j)
                    {
                        if(0 == p[j].busy)
                        {
                            sendFd(p[j].fd, new_fd); //把连接上的new_fd传给空闲的子进程
                            p[j].busy=1;
                            printf("%d is busy.\n", p[j].pid);
                            break;
                        }
                    }
                    close(new_fd);
                }
                else if(1==ret) //如果存在此token,则说明是getsputs认证连接
                {
                    //printf("客户端子线程认证成功\n");
                    tNode.new_fd[tNode.fdcnt++]=new_fd; //此处操作要加锁，待改
                    if(tNode.fdcnt >= tNode.fdMax-1)    //使用动态数组存在内存泄漏，待改成动态链表
                    {
                        tNode.fdMax += 10;
                        tNode.new_fd=(int*)realloc(tNode.new_fd, tNode.fdMax*sizeof(int));
                    }
                    //usleep(1000);  //保证fds[]在子线程那边已注册
                    write(tNode.fds[1], (char*)&ret, 4); //通知子线程
                }
                else
                {
                    printf("认证失败\n"); //暂时只有存在与不存在token值，token值匹配错误的情况待补
                }
            }       
            for(j=0;j<childNum;++j)
            {
                if(evs[i].data.fd==p[j].fd)
                {
                    p[j].busy=0;
                    read(p[j].fd, &ret, 1);//接子进程发过来的信号
                    //printf("%d misson completed\n", p[j].pid);
                    break;
                }
            }
        }
    }
}
