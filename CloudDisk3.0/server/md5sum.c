#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<openssl/md5.h>

int getMD5(char *md5sum, int fd)
{
    int i;
    int ret;
    unsigned char data[1024];
    unsigned char md5_value[16];
    unsigned char md5_str[33];
    MD5_CTX md5;
    MD5_Init(&md5);
    while(1)
    {
        ret = read(fd, data, 1024);
        if(-1==ret)
        {
            perror("read");
            return -1;
        }
        MD5_Update(&md5, data, ret);
        if(0==ret || ret<1024)
        {
            break;
        }
    }
    MD5_Final(md5_value, &md5);
    for(i=0;i<16;++i)
    {
        snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
    }
    md5_str[32]='\0';
    //printf("%s\n", md5_str);
    strcpy(md5sum, md5_str);
    return 0;
}
