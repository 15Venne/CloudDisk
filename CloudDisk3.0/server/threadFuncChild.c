#include"head.h"

//暂不考虑关掉和解注册太久不发请求的描述符


void *threadFuncChild(void *p)
{
    threadNode *pt=(threadNode*)p;
    int ret, evscnt=1;
    int orderNum;
    int epfdChild = epoll_create(1);
    struct epoll_event eventChild, *evsChild;
    evsChild = (struct epoll_event*)calloc(evscnt, sizeof(struct epoll_event));

    //以下注册fds[0]
    eventChild.events = EPOLLIN;
    eventChild.data.fd = pt->fds[0];
    ret = epoll_ctl(epfdChild, EPOLL_CTL_ADD, pt->fds[0], &eventChild);
    if(-1==ret){ perror("epoll_ctl"); }
    int i, j, readyNum;
    char filename[50];
    char path[100];
    while(1)
    {
        readyNum = epoll_wait(epfdChild, evsChild, evscnt, -1);
        if(-1==readyNum) { perror("epoll_wait"); }
        for(i=0;i<readyNum;++i)
        {
            if(evsChild[i].data.fd == pt->fds[0]) //如果主线程发信号，则有客户端子线程连接
            {
                read(pt->fds[0], (char*)&ret, 4);
                //假如pt->fdcnt为1，则new_fd[0]为新增，注册此fd
                //即注册new_fd[fdcnt-1]
                //printf("子线程被唤醒，pt->new_fd=%d\n", pt->new_fd[0]);
                eventChild.events = EPOLLIN;
                eventChild.data.fd = pt->new_fd[pt->fdcnt-1];
                ret = epoll_ctl(epfdChild, EPOLL_CTL_ADD, pt->new_fd[pt->fdcnt-1], &eventChild);
                if(-1==ret) { perror("epoll_ctl_pt->new_fd"); }
                evscnt++; //注册数++
                //printf("客户端子线程已注册,ret=%d\n", ret);
            }
            for(j = 0; j < pt->fdcnt; ++j)
            {
                if(evsChild[i].data.fd == pt->new_fd[j]) //如果客户端发来请求
                {
                    memset(filename, 0, sizeof(filename));
                    memset(path, 0, sizeof(path));
                    ret = recv(pt->new_fd[j], (char*)&orderNum, 4, 0);
                    if(0==ret) // 客户端退出，解注册fd, 关掉fd
                    {
                        eventChild.events = EPOLLIN;
                        eventChild.data.fd = pt->new_fd[j];
                        epoll_ctl(epfdChild, EPOLL_CTL_DEL, pt->new_fd[j], &eventChild);
                        close(pt->new_fd[j]);
                        break;
                    }
                    recv(pt->new_fd[j], (char*)&ret, 4, 0);
                    recv(pt->new_fd[j], filename, ret, 0);
                    recv(pt->new_fd[j], (char*)&ret, 4, 0);
                    recv(pt->new_fd[j], path, ret, 0);
                    //printf("orderNum=%d, filename:%s\n", orderNum, filename);
                    //printf("path:%s\n", path);
                    if(PUTS_CMD == orderNum)
                    {
                        ret = recvFile(path, filename, pt->new_fd[j]);
                        if(-1==ret)
                        {
                            printf("client upload failed\n");
                        }
                    }
                    else if(GETS_CMD == orderNum)
                    {
                        ret = tranFile(path, filename, pt->new_fd[j]);
                        if(-1==ret)
                        {
                            printf("client download failed\n");
                        }
                    }
                    break;
                }
            }
        }
    }
}
