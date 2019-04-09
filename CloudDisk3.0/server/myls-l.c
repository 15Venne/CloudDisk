#include"head.h"
typedef struct fileInfo
{
    char name[50];
    struct stat *p;
}fileInfo;

int filecnt =0;
//建文件数组
void makeFilearr(fileInfo *fileArr,char *name,struct stat *p)
{
    strcpy(fileArr[filecnt].name,name);
    fileArr[filecnt].p=(struct stat*)malloc(sizeof(struct stat));
    *fileArr[filecnt].p=*p;
    filecnt++;
}

char changeAlp(char c)
{

    if(c >= 'A' && c <= 'Z')
        return c+32;
    else
        return c;
}

//无视字母大小写的字符串排序
int mystrcmp(char *p1,char *p2)
{
    int i=0;
    while(*p1 != '\0' && *p2 != '\0')
    {
        if(changeAlp(p1[i])>changeAlp(p2[i]))
            return 1;
        else if(changeAlp(p1[i])<changeAlp(p2[i]))
            return -1;
        else
        {
            p1++;
            p2++;
        }
    }
    if(p1[i]=='\0'&&p2[i]=='\0')
        return 0;
    if(p1[i]=='\0')
        return -1;
    if(p2[i]=='\0')
        return 1;
}

//快排的compare
int compare(const void *a,const void *b)
{
    fileInfo *p=(fileInfo*)a;
    fileInfo *q=(fileInfo*)b;
    if(mystrcmp(p->name,q->name)<0)
        return -1;
    else if(mystrcmp(p->name,q->name)>0)
        return 1;
    else
        return 0;
}

int fileinfoPrint(struct stat *p)
{
         //           文件大小
    return 0;
}


int myls(char *pdir, int new_fd, char *usr_name)
{
    struct dirent *p;                                             
    char buf[1024]={0};                                           
    int ret;                                                      
    struct stat cur;                                              
    fileInfo fileArr[100]={0}; //文件数组，排序用                 
    char *oripwd = getcwd(NULL, 0);//记录一开始的路径
    chdir(pdir);   //切换当前工作目录                          
    char *chpwd=getcwd(NULL,0); //获取当前绝对路径   
    
    //以下判断该路径是否在该用户下
    ret=0;
    while(chpwd[ret]!='r')
    {
        ret++;
    }
    chpwd=chpwd+ret+2;
    ret=0;
    while(chpwd[ret]!='\0' && chpwd[ret]!='/')
    {
        ret++;
    }
    chpwd[ret]='\0';
    //printf("chpwd:%s\n", chpwd);
    //printf("usr_name:%s\n", usr_name);
    ret=strcmp(chpwd, usr_name);
    send(new_fd, (char*)&ret, 4, 0);
    if(ret!=0)
    {
        //printf("请求不合法\n");
        chdir(oripwd);
        return -1;
    }
    chpwd=getcwd(NULL, 0);
    //printf("%s\n",chpwd);   //打印当前绝对路径                    
    DIR *dir=opendir(chpwd); //获取DIR                            
    if(NULL==dir)                                                 
    {                                                             
        perror("opendir");                                        
    }                                                             
    while((p=readdir(dir))!=NULL)// 依次读取目录下文件            
    {                                                             
        if('.' == p->d_name[0])  // 如果是隐藏文件，continue      
        {                                                         
            continue;                                             
        }                                                            
        sprintf(buf,"%s/%s",chpwd,p->d_name); //拼接文件路径      
        ret=stat(buf,&cur);                   //获取文件stat信息  
        if(-1==ret)                           //返回给cur结构体   
        {                                                         
            return -1;                                            
        }                                                         
        makeFilearr(fileArr,p->d_name,&cur);                      
    }                                                             
    //给文件数组排序      
    qsort(fileArr, filecnt, sizeof(fileInfo), compare);
    int dataLen;
    for(ret=0;ret<filecnt;++ret)                                
    {   
        memset(buf, 0, sizeof(buf));
        if(fileArr[ret].p->st_mode/10000 == 1)
        {
          //  printf("d  ");
            strcpy(buf, "d  ");
        }
        else
        {
           // printf("f  ");
            strcpy(buf, "f  ");
        }
        //printf("%-20s",fileArr[ret].name); 
        sprintf(buf, "%s%-20s%5ld", buf, fileArr[ret].name, fileArr[ret].p->st_size);
        //printf("%5ld\n",fileArr[ret].p->st_size);
        //printf("%s\n", buf);
        dataLen=strlen(buf);
        sendPlus(new_fd, (char*)&dataLen, 4);
        sendPlus(new_fd, buf, dataLen);
        free(fileArr[ret].p);
    }
    dataLen=0;
    sendPlus(new_fd, (char*)&dataLen, 4);
    closedir(dir); 
    chdir(oripwd);
    filecnt=0;
    return 0;  
}
