#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<shadow.h>
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
#include<sys/sendfile.h>
#include<pthread.h>

#define myport 2015

#define LS_CMD 1
#define CD_CMD 2
#define PUTS_CMD 3
#define GETS_CMD 4
#define REMOVE_CMD 5
#define PWD_CMD 6
#define MKDIR_CMD 7

typedef struct train
{
    int dataLen;
    char buf[1000];
}train;

typedef struct tokenNode
{
    char token[9];
    int orderNum;
    char buf[50];
    char path[100];
    char ip[16];
    int port;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
}tokenNode,*ptokenNode;

int sendPlus(int, char*, int);

int recvPlus(int, char*, int, off_t*, int);

int tcpConnect(int*, const char*, int);

int orderCheck(char*, char*);

int lsrecv(int);

int cdrecv(int);
int putsrecv(int, char*);
int getsrecv(int, char*);
int removerecv(int);
int pwdrecv(int);
int mkdirrecv(int);

int is_sameFile_exist(char*, off_t*);

int getMD5(char*, int);

void *threadFuncGetsPuts(void*);
