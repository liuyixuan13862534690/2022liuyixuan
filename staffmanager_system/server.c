#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define DBNAME "stu.db"
#define TABLE "accountmessage"
#define TABLE_staff "staffmessage"
typedef struct{
	int type;         //代表给服务器发送的功能，作为判断注册和登录
	char account[32];//账号
	char password[32];//密码
	int re_flag;//判断是否注册成功的标志位
	int flag;//判断是否登录成功的标志位
	int nrow;//判断是否查询成功的标志位
	char name[32];
	char sex[32];//M  or W
	int age;
	int salary;//薪资
	char depart[32];//部门
	int phone;
}MSG;
void do_client(int acceptfd,sqlite3*db);
void do_register(int connectfd,MSG*msg,sqlite3*db);
void do_login(int connectfd,MSG*msg,sqlite3*db);
void do_insert(int connectfd,MSG*msg,sqlite3*db);
void do_delete(int connectfd,MSG*msg,sqlite3*db);
void do_search(int connectfd,MSG*msg,sqlite3*db);
void do_change(int connectfd,MSG*msg,sqlite3*db);
void do_usr_search(int connectfd,MSG*msg,sqlite3*db);
void handler(int sig)
{
    wait(NULL);
}
int main(int argc, const char *argv[])
{
	int listenfd, connectfd;
	struct sockaddr_in server_addr;
	pid_t pid;
	sqlite3 *db;
    if (argc < 3)
	{
		printf("Usage : %s <ip> <port>\n", argv[0]);
		exit(-1);
	}
	if (sqlite3_open(DBNAME, &db) != SQLITE_OK)
	{
		printf("error : %s\n", sqlite3_errmsg(db));
		exit(-1);
	}

	sqlite3_exec(db, "create table accountmessage(account text primary key, password text)", NULL, NULL, NULL);
	sqlite3_exec(db, "create table staffmessage(name text, sex text, age text ,salary text,depart text,phone text)", NULL, NULL, NULL);

	if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("fail to socket");
		exit(-1);
	}

    bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("fail to bind");
		exit(-1);
	}

    if (listen(listenfd, 5) < 0)
	{
		perror("fail to listen");
		exit(-1);
	}

    signal(SIGCHLD, handler);//处理僵尸进程

    while ( 1 )
	{
		if ((connectfd = accept(listenfd, NULL, NULL)) < 0)
		{
			perror("fail to accept");
			exit(-1);
		}

        if ((pid = fork()) < 0)
		{
			perror("fail to fork");
			exit(-1);
		}
		else if(pid == 0) //子进程执行处理代码
		{
			do_client(connectfd, db);
		}
        else  //父进程负责连接
        {
		    close(connectfd);
        }
	}
	return 0;
}
void do_client(int connectfd,sqlite3*db)
{
	MSG msg;
	while (recv(connectfd, &msg, sizeof(MSG), 0) > 0)  // receive request
	{
		printf("type=%d\n",msg.type);
		printf("password=%s\n",msg.password);
		switch(msg.type)
		{
			case 1:
				do_register(connectfd,&msg,db);
				break;
			case 2:
				do_login(connectfd,&msg,db);
				break;
			case 4:
				do_usr_search(connectfd,&msg,db);
				break;
			case 10:
				do_insert(connectfd,&msg,db);
				break;
			case 20:
				do_delete(connectfd,&msg,db);
				break;
			case 30:
				do_change(connectfd,&msg,db);
				break;
			case 40:
				do_search(connectfd,&msg,db);
				break;
			case 50:
				close(connectfd);
				exit(1);
		}

	}
return;
}
void do_register(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg;
	sprintf(sql,"insert into accountmessage values('%s','%s')",msg->account,msg->password);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("%s已经存在了，插入失败\n",msg->account);
	}
	else
	{
		msg->re_flag=1;
	}
	send(connectfd,msg,sizeof(MSG),0);
	return;
}
void do_login(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg,**result;
	int exsit,ncolumn;
	sprintf(sql, "select * from accountmessage where account = '%s' and password = '%s'", msg->account, msg->password);
	if(sqlite3_get_table(db, sql, &result, &exsit, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
	}
    //通过nrow参数判断是否能够查询到记录，如果值为0，则查询不到，如果值为非0，则查询到
	if(exsit == 0)
	{
		printf("此人不存在，请重新注册\n");
		msg->flag=2;
		send(connectfd,msg,sizeof(MSG),0);
	}
	else
	{
		msg->flag=1;
		send(connectfd,msg,sizeof(MSG),0);
	}
	return ;
}
void do_insert(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg;
	sprintf(sql,"insert into staffmessage values('%s','%s',%d,%d,'%s',%d)",msg->name,
			msg->sex,msg->age,msg->salary,msg->depart,msg->phone);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("增加error : %s\n", errmsg);
	}
	send(connectfd,msg,sizeof(MSG),0);
	return;
}

void do_delete(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg;
	sprintf(sql,"delete  from staffmessage where  name='%s'",msg->name);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("删除error : %s\n", errmsg);
	}
	send(connectfd,msg,sizeof(MSG),0);
	return;

}
void do_change(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg,**result;
	int exsit,ncolumn;
	sprintf(sql,"select * from staffmessage where name='%s'",msg->name);
	if(sqlite3_get_table(db, sql, &result, &exsit, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
	}
	strcpy(msg->name,result[6]);
	strcpy(msg->sex,result[7]);
	msg->age=atoi(result[8]);
	msg->salary=atoi(result[9]);
	strcpy(msg->depart,result[10]);
	msg->phone=atoi(result[11]);
	msg->nrow=1;
	send(connectfd,msg,sizeof(MSG),0);
	recv(connectfd,msg,sizeof(MSG),0);
	char sqlstr[512]={0};
	sprintf(sqlstr,"update staffmessage set salary=%d  where name='%s'",msg->salary,msg->name);
	if(sqlite3_exec(db,sqlstr,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("修改error : %s\n", errmsg);
	}
	else
		return ;
	
}
void do_search(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg,**result;
	int exsit,ncolumn;
	sprintf(sql,"select * from staffmessage where name='%s'",msg->name);
	if(sqlite3_get_table(db, sql, &result, &exsit, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
	}
	strcpy(msg->name,result[6]);
	strcpy(msg->sex,result[7]);
	msg->age=atoi(result[8]);
	msg->salary=atoi(result[9]);
	strcpy(msg->depart,result[10]);
	msg->phone=atoi(result[11]);
	msg->nrow=1;
	send(connectfd,msg,sizeof(MSG),0);
	return;
}
void do_usr_search(int connectfd,MSG*msg,sqlite3*db)
{
	char sql[512]={0};
	char *errmsg,**result;
	int exsit,ncolumn;
	sprintf(sql,"select * from staffmessage where name='%s'",msg->name);
	if(sqlite3_get_table(db, sql, &result, &exsit, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
	}
	strcpy(msg->name,result[6]);
	strcpy(msg->sex,result[7]);
	msg->age=atoi(result[8]);
	msg->salary=atoi(result[9]);
	strcpy(msg->depart,result[10]);
	msg->phone=atoi(result[11]);
	msg->nrow=1;
	send(connectfd,msg,sizeof(MSG),0);
	return;

}
#if 0
void do_usr_change(int connectfd,MSG*msg,sqlite3*db)
{
	char sqlstr[512]={0};
	char *errmsg;
	sprintf(sqlstr,"update staffmessage set sex='%s' where name='%s'",msg->sex,msg->name);
	if(sqlite3_exec(db,sqlstr,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("修改error : %s\n", errmsg);
	}
	sprintf(sqlstr,"update staffmessage set age=%d where name='%s'",msg->age,msg->name);
	if(sqlite3_exec(db,sqlstr,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("修改error : %s\n", errmsg);
	}
	sprintf(sqlstr,"update staffmessage set phone=%d where name='%s'",msg->phone,msg->name);
	if(sqlite3_exec(db,sqlstr,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("修改error : %s\n", errmsg);
	}
	else
		return ;
	
}
#endif
