#include"head.h"

int requestDeal(char *buf, char *buf1)
{
    int i=0;
    while(buf[i]!=' ' && buf[i]!='\0')
    {
        i++;
    }
    if(buf[i]==' ')
    {
        strcpy(buf1, buf+i+1);
        buf[i]='\0';
    }
    if(0==strcmp(buf, "ls"))
    {
        return LS_CMD;
    }
    else if(0==strcmp(buf, "cd"))
    {
        return CD_CMD;
    }
    else if(0==strcmp(buf, "puts"))
    {
        return PUTS_CMD;
    }
    else if(0==strcmp(buf, "gets"))
    {
        return GETS_CMD;
    }
    else if(0==strcmp(buf, "remove"))
    {
        return REMOVE_CMD;
    }
    else if(0==strcmp(buf, "pwd"))
    {
        return PWD_CMD;
    }
    else if(0==strcmp(buf, "mkdir"))
    {
        return MKDIR_CMD;
    }
    else  return -1;
}

int missonStart(int new_fd)
{
    char buf[200]={0};
    char buf1[200]={0};
    char user_name[50]={0};
    int ret, order;
    int dataLen;
Loop:
    memset(buf, 0, sizeof(buf));
    memset(buf1, 0, sizeof(buf1));
    recv(new_fd, (char*)&dataLen, 4, 0);
    recv(new_fd, buf, dataLen, 0);//接收用户名
    strcpy(user_name, buf);
    printf("%s尝试登陆\n", user_name);
    ret=getCiphertext(buf, buf1);//buf返回盐值，buf1返回密文
    //printf("ret=%d\n密文buf1:%s\n", ret, buf1);
    dataLen=strlen(buf);
    send(new_fd, (char*)&dataLen, 4, 0);
    send(new_fd, buf, dataLen, 0); //发送盐值给客户端

    memset(buf, 0, sizeof(buf));
    recv(new_fd, (char*)&dataLen, 4, 0);
    recv(new_fd, buf, dataLen, 0); //接收客户端的密文
    //以下配对密文
    ret = strcmp(buf, buf1);
    send(new_fd, (char*)&ret, 4, 0);//发送密文是否配对成功标志
    if(ret!=0)//配对不成功
    {
       printf("%s用户名密码错误\n", user_name);
       goto Loop;
    }
    printf("%s用户登陆成功\n", user_name);
    ret = insertTable_ConnectInfo(user_name);//数据库记录用户连接时间
    if(ret!=0)
    {
        printf("插入失败\n");
    }
    char token[9]={0};
    GenerateStr(token); //生成token值，正确做法是把用户名加密生成，以后改良
    //printf("生成token:%s\n", token);
    insertTable_userToken(user_name, token);
    send(new_fd, token, 9, 0);//发送token值
    
    memset(buf, 0, sizeof(buf));
    strcpy(buf, "/home/venne/CloudDisk/usr");
    sprintf(buf, "%s/%s", buf, user_name);
    chdir(buf);
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        memset(buf1, 0, sizeof(buf1));
        strcpy(buf, getcwd(NULL, 0));
        ret=0;
        while(buf[ret]!='r')
        {
            ret++;
        }
        strcpy(buf, buf+ret+2);
        dataLen=strlen(buf); 
        send(new_fd, (char*)&dataLen, 4, 0); //发送路径
        send(new_fd, buf, dataLen, 0);
        
        memset(buf, 0, sizeof(buf));
    
        recv(new_fd, (char*)&dataLen, 4, 0);
        ret = recv(new_fd, buf, dataLen, 0);//收到请求
        if(0 == ret)
        {
            printf("client loses connection\n");
            //chdir("/home/venne/Linux/20190329/server");
            //这里要写删除token值
            deleteTable_userToken(user_name);
            break;
        }
        else if(-1==ret) { perror("recv"); }
        printf("收到%s请求 %s\n",user_name, buf);
        order=requestDeal(buf, buf1);//切割请求,buf为命令 
        //printf("buf=%s buf1=%s\n", buf,buf1);
        //printf("order=%d\n", order);
        if(order == LS_CMD)
        {
            ret = myls(buf1, new_fd, user_name);
            sprintf(buf, "%s %s", buf, buf1);
            insertTable_Operation(user_name, buf, ret);//数据库记录用户操作
        }
        else if(PWD_CMD == order)
        {
            ret = mypwd(new_fd);
            insertTable_Operation(user_name, buf, ret);//数据库记录用户操作
        }
        else if(CD_CMD == order)
        {
            ret = mycd(buf1, new_fd, user_name);
            sprintf(buf, "%s %s", buf, buf1);
            insertTable_Operation(user_name, buf, ret);//数据库记录用户操作
        }
        else if(REMOVE_CMD == order)
        {
            ret = myremove(buf1, new_fd);
            sprintf(buf, "%s %s", buf, buf1);
            insertTable_Operation(user_name, buf, ret);//数据库记录用户操作
        }
        else if(MKDIR_CMD == order)
        {
            ret = mymkdir(buf1, new_fd);
            sprintf(buf, "%s %s", buf, buf1);
            insertTable_Operation(user_name, buf, ret);//数据库记录用户操作
        }
        /* else if(GETS_CMD == order) */
        /* { */
            //ret = tranFile(buf1, new_fd);
            //if(-1 == ret)
            /* { */
            /*     printf("client download failed\n"); */
            /* } */
            /* sprintf(buf, "%s %s", buf, buf1); */
            /* insertTable_Operation(user_name, buf, ret);//数据库记录用户操作 */
        /* } */
        /* else if(PUTS_CMD == order) */
        /* { */
            /* ret = recvFile(buf1, new_fd); */
            /* if(-1 == ret) */
            /* { */
            /*     printf("client upload failed\n"); */
            /* } */
            /* sprintf(buf, "%s %s", buf, buf1); */
            /* insertTable_Operation(user_name, buf, ret);//数据库记录用户操作 */
        /* } */
    }
    return 0;
}
