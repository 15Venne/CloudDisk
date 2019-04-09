#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<pthread.h>
#include<sys/epoll.h>
#include<grp.h>
#include<pwd.h>
#include<dirent.h>
#include<errno.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/uio.h>
#include<mysql/mysql.h>
#include<pthread.h>

#define myport 2015 //默认端口2015
#define childNum 5  //默认子进程5

#define LS_CMD 1
#define CD_CMD 2
#define PUTS_CMD 3
#define GETS_CMD 4
#define REMOVE_CMD 5
#define PWD_CMD 6
#define MKDIR_CMD 7


typedef struct ProcessData
{
    pid_t pid;
    int fd;
    short busy;
}ProcessData;

typedef struct train
{
    int dataLen;
    char buf[1000];
}train;

typedef struct threadNode
{
    int *new_fd; //new_fd数组首地址
    int fdMax;   //当前数组最大个数
    int fdcnt;     //new_fd数组个数
    int fds[2];  //pipe
}threadNode,*pthreadNode;

int sendPlus(int, char*, int);

int recvPlus(int, char*, int);

int makeChild(ProcessData*, int);

int recvFd(int, int *fd);

int sendFd(int, int fd);

int tcpInit(int*, char*, int);

int missonStart(int new_fd);

int requestDeal(char*, char*);

int myls(char*, int, char*);
int mypwd(int);
int mycd(char*, int, char*);
int myremove(char*, int);
int mymkdir(char*, int);
int tranFile(char*, char*, int);
int recvFile(char*, char*, int);

int getCiphertext(char*, char*);
int insertTable_ConnectInfo(char*);
int insertTable_Operation(char*, char*, int);
int insertTable_userToken(char*, char*);
int deleteTable_userToken(char*);


int is_sameFile_exist(char*, char*, off_t*);
int getMD5(char*, int);
int checkToken(char*);

int GenerateStr(char*);

void *threadFuncChild(void*);
