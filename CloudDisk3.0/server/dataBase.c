#include"head.h"

int deleteTable_userToken(char *user_name)
{
    MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="0928";
	char* database="CloudDisk";
	char query[200]="delete from userToken where userName='";
    sprintf(query, "%s%s'", query, user_name);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		//printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		//printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(conn));
	}
	mysql_close(conn);
	return 0;   
}

int insertTable_userToken(char *user_name, char *token)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";   
    char *user="root";          
    char *password="0928";      
    char query[300]={0}; 
    char *database="CloudDisk"; 
    strcpy(query, "insert into userToken (userName, Token) values('");
    sprintf(query, "%s%s', '%s')", query, user_name, token);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting:%s\n", mysql_error(conn));
        return -1;
    }
    t=mysql_real_query(conn,query, strlen(query));
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
        return -1;
    }
    else
    {
       // printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}
int insertTable_ConnectInfo(char*user_name)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";   
    char *user="root";          
    char *password="0928";      
    char query[300]={0}; 
    char *database="CloudDisk"; 
    strcpy(query, "insert into userConnectInfo (name) values('");
    sprintf(query, "%s%s%s", query, user_name, "')");
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting:%s\n", mysql_error(conn));
        return -1;
    }
    t=mysql_real_query(conn,query, strlen(query));
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
        return -1;
    }
    else
    {
        //printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}

int insertTable_Operation(char *user_name, char *operation, int flag)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";   
    char *user="root";          
    char *password="0928";      
    char query[300]={0}; 
    char *database="CloudDisk"; 
    strcpy(query, "insert into userLog (name, operation, flag) values('");
    sprintf(query, "%s%s', '%s', %d)", query, user_name, operation, flag);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting:%s\n", mysql_error(conn));
        return -1;
    }
    t=mysql_real_query(conn,query, strlen(query));
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
        return -1;
    }
    else
    {
       // printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}

int getCiphertext(char *name, char *ciptext) //名字返回盐值
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";
    char *user="root";
    char *password="0928";
    char query[300]={0};
    char *database="CloudDisk";
    strcpy(query, "select * from user where name='");
    sprintf(query, "%s%s%s", query, name, "'");
    int t, r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting\n");
        return -1;
    }
    //printf("connected\n");

    t=mysql_real_query(conn,query, strlen(query));
    //t=mysql_query(conn,query);
    //printf("t=%d\n", t);
    if(t)
    {
        printf("error\n");
        mysql_close(conn);
        return -1;
    }
    else
    {
        res = mysql_use_result(conn);
        row = mysql_fetch_row(res);
        if(row!=NULL)
        {
            //row=mysql_fetch_row(res);
            memset(name, 0, sizeof(name));
            strcpy(name, row[2]);
            //printf("%s\n", row[2]); //盐值
            strcpy(ciptext, row[3]);
            //printf("%s\n", row[3]);   //密文
            mysql_free_result(res);
            mysql_close(conn);
            return 0;
        }
        else
        {
            printf("no user data\n");
            mysql_free_result(res);
            mysql_close(conn);
            return -1;

        }
    }
}

int checkToken(char *token)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";
    char *user="root";
    char *password="0928";
    char query[300]={0};
    char *database="CloudDisk";
    strcpy(query, "select * from userToken where token='");
    sprintf(query, "%s%s'", query, token);
    int t, r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting\n");
        return -1;
    }
    //printf("connected\n");

    t=mysql_real_query(conn,query, strlen(query));
    //t=mysql_query(conn,query);
    //printf("t=%d\n", t);
    if(t)
    {
        printf("error\n");
        mysql_close(conn);
        return -1;
    }
    else
    {
        res = mysql_use_result(conn);
        row = mysql_fetch_row(res);
        if(row!=NULL)//有此token值
        {
            //strcpy(name, row[2]);
            mysql_free_result(res);
            mysql_close(conn);
            return 1;
        }
        else //客户端登陆也是返回0，导致token值错误的情况也会转到登陆协议，待改良
        {
            //printf("no user data\n");
            mysql_free_result(res);
            mysql_close(conn);
            return 0;

        }
    }
}
