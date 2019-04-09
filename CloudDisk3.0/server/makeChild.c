#include"head.h"

int sendFd(int pipeFd, int fd)
{
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    struct iovec iov[2];
    iov[0].iov_base = "ha";
    iov[0].iov_len = 2;
    iov[1].iov_base = "lo";
    iov[1].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    int cmsgLen = CMSG_LEN(sizeof(int)); //变长结构体长度

    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1,  cmsgLen);
    cmsg->cmsg_len = cmsgLen;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *(int*)CMSG_DATA(cmsg)=fd;

    msg.msg_control = cmsg; //设置好的cmsg结构体指针给msg.msg_control
    msg.msg_controllen = cmsgLen;

    int ret;
    ret = sendmsg(pipeFd, &msg, 0);
    if(-1 == ret) {
        perror("sendmsg");
    }
    return 0;
}

int recvFd(int pipeFd, int *fd)
{
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    struct iovec iov[2];
    char buf1[5], buf2[5];
    iov[0].iov_base = buf1;
    iov[0].iov_len = 2;
    iov[1].iov_base = buf2;
    iov[1].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    int cmsgLen = CMSG_LEN(sizeof(int)); //变长结构体长度

    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1, cmsgLen);
    cmsg->cmsg_len = cmsgLen;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    msg.msg_control = cmsg; //设置好的cmsg结构体指针给msg.msg_control
    msg.msg_controllen = cmsgLen;

    int ret;
    ret = recvmsg(pipeFd, &msg, 0);
    if(-1 == ret)
    {
        perror("sendmsg");
    }
    *fd = *(int*)CMSG_DATA(cmsg);
    return 0;
}

int makeChild(ProcessData *p, int num)
{
    pid_t pid;
    int fds[2], i, ret;
    int new_fd;
    for(i=0;i<num;++i)
    {
        ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
        pid = fork();
        if(0==pid)  //子进程，处理任务
        {
            close(fds[1]);
            while(1)
            {
                recvFd(fds[0], &new_fd);
                missonStart(new_fd);
                close(new_fd);
                write(fds[0], &new_fd, 1); //告诉父进程完成任务
            }
        }
        else //父进程
        {
            close(fds[0]);
            p[i].pid=pid;
            p[i].fd=fds[1];
        }
    }
}
