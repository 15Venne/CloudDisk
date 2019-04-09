//用户登陆成功后，生成token

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#define STR_LEN 8

int GenerateStr(char *ranNum)
{
    char str[STR_LEN+1]={0};
    int i, flag;
    srand(time(NULL));

    for(i=0;i<STR_LEN;++i)
    {
        flag = rand()%3;
        switch(flag)
        {
        case 0:
            str[i]=rand()%26+'a';
            break;
        case 1:
            str[i]=rand()%26+'A';
            break;
        case 2:
            str[i]=rand()%10+'0';
            break;
        }
    }
    strcpy(ranNum, str);
    //printf("生成的随机数%s\n",str);
}

