#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
void do_regiseter(int socketfd,MSG*msg);
void do_login(int socketfd,MSG*msg);
void register_print();
void user_login_print();
void manager_login_print();
void manager_insert(int socketfd,MSG*msg);
void manager_delete(int socketfd,MSG*msg);
int manager_search(int socketfd,MSG*msg);
void manager_change(int socketfd,MSG*msg);
void usr_search(int socketfd,MSG*msg);
int main(int argc, const char *argv[])
{
	if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
	int socketfd;
	struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);
	MSG msg;
	//第一步，socket套接字
	if((socketfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket error");
	}
	//第二步，填充网络信息的结构体
	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(atoi(argv[2]));

	//第三步，connect连接服务器
	if(connect(socketfd,(struct sockaddr*)&serveraddr,addrlen)==-1)
	{
		perror("connect error");
	}
	while(1)
	{
		int i;
		register_print();
		scanf("%d",&i);
		switch(i)
		{
		case 1:
			do_regiseter(socketfd,&msg);
			break;
		case 2:
			do_login(socketfd,&msg);
			break;
		case 3:
			close(socketfd);
			exit(0);
		}

	}

	return 0;
}
void do_regiseter(int socketfd,MSG*msg)
{
	msg->type=1;
	printf("请输入你的账号");
	scanf("%s",msg->account);
	printf("请输入你的密码");
	scanf("%s",msg->password);
	send(socketfd,msg,sizeof(MSG),0);//发送数据
	printf("注册账号密码已经发送\n");
	recv(socketfd,msg,sizeof(MSG),0);//接收数据
	if(msg->re_flag==1)
	{
		printf("注册成功了\n");
	}

	return;
}
void do_login(int socketfd,MSG*msg)
{
	msg->type=2;
	printf("请输入你的账号");
	scanf("%s",msg->account);
	printf("请输入你的密码");
	scanf("%s",msg->password);
	send(socketfd,msg,sizeof(MSG),0);//发送数据
	recv(socketfd,msg,sizeof(MSG),0);//接收数据
	if(msg->flag==1)
	{
		printf("你登录成功了.");
		if(strncmp(msg->account,"manager",7)==0)
		{
			int choose;
NEXT:
			manager_login_print();
			scanf("%d",&choose);
			switch(choose)
			{
				case 10:
					manager_insert(socketfd,msg);
					goto NEXT;
					break;
				case 20:
					manager_delete(socketfd,msg);
					goto NEXT;
					break;
				case 30:
					manager_change(socketfd,msg);
					goto NEXT;
					break;
				case 40:
					manager_search(socketfd,msg);
					goto NEXT;
					break;
				case 50:
					close(socketfd);
					exit(1);
			}
			
		}
		else
		{
			int usr_choose;
NEXT1:
			user_login_print();
			scanf("%d",&usr_choose);
			switch(usr_choose)
			{
				case 4:
					usr_search(socketfd,msg);
					goto NEXT1;
					break;
				case 6:
					close(socketfd);
					exit(1);
			}

		}
		 if(msg->flag==2)
		{
			printf("你登录失败了");
		}
	}
	return;
}
void manager_insert(int socketfd,MSG*msg)
{
	msg->type=10;
	printf("请输入你的名字");
	scanf("%s",msg->name);
	getchar();
	printf("请输入你的性别");
	scanf("%s",msg->sex);
	getchar();
	printf("请输入你的年龄");
	scanf("%d",&msg->age);
	printf("请输入你的薪资");
	scanf("%d",&msg->salary);
	printf("请输入你的部门");
	scanf("%s",msg->depart);
	getchar();
	printf("请输入你的联系方式");
	scanf("%d",&msg->phone);
	send(socketfd,msg,sizeof(MSG),0);
	recv(socketfd,msg,sizeof(MSG),0);
}
void manager_delete(int socketfd,MSG*msg)
{
	msg->type=20;
	printf("请输入你要删除人员的姓名\n");
	scanf("%s",msg->name);
	getchar();
	send(socketfd,msg,sizeof(MSG),0);
	recv(socketfd,msg,sizeof(MSG),0);
}
void manager_change(int socketfd,MSG*msg)
{
	msg->type=30;
	printf("请输入你要修改人员的姓名\n");
	scanf("%s",msg->name);
	getchar();
	send(socketfd,msg,sizeof(MSG),0);
	recv(socketfd,msg,sizeof(MSG),0);
	if(msg->nrow==1)
	{
		printf("此人存在，请输入你要更新的薪资\n");
		scanf("%d",&msg->salary);
		send(socketfd,msg,sizeof(MSG),0);
	}
	
}
int manager_search(int socketfd,MSG*msg)
{
	msg->type=40;
	printf("请输入你要查询人员的姓名");
	scanf("%s",msg->name);
	getchar();
	send(socketfd,msg,sizeof(MSG),0);
	recv(socketfd,msg,sizeof(MSG),0);
	if(msg->nrow==1)
	{
		printf("查询后，姓名为:%s\n",msg->name);
		printf("查询后，性别为:%s\n",msg->sex);
		printf("查询后，年龄为:%d\n",msg->age);
		printf("查询后，薪资为:%d\n",msg->salary);
		printf("查询后，部门为:%s\n",msg->depart);
		printf("查询后，电话为:%d\n",msg->phone);
		msg->nrow=0;
		return 1;
	}
	else
		printf("查无此人\n");
	return 0;
}
void usr_search(int socketfd,MSG*msg)
{
	msg->type=4;
	printf("请输入你的姓名");
	scanf("%s",msg->name);
	getchar();
	send(socketfd,msg,sizeof(MSG),0);
	recv(socketfd,msg,sizeof(MSG),0);
	if(msg->nrow==1)
	{
		printf("查询后，姓名为:%s\n",msg->name);
		printf("查询后，性别为:%s\n",msg->sex);
		printf("查询后，年龄为:%d\n",msg->age);
		printf("查询后，薪资为:%d\n",msg->salary);
		printf("查询后，部门为:%s\n",msg->depart);
		printf("查询后，电话为:%d\n",msg->phone);
		msg->nrow=0;
		return ;
	}
	else
		printf("你的信息不存在\n");
	return ;
	

}
#if 0
void usr_change(int socketfd,MSG*msg)
{
	msg->type=30;
	printf("请输入你的姓名\n");
	scanf("%s",msg->name);
	getchar();
	printf("请输入你要更新的性别\n");
	scanf("%s",msg->sex);
	getchar();
	printf("请输入你要更新的年龄\n");
	scanf("%d",&msg->age);
	printf("请输入你要更新的电话\n");
	scanf("%d",&msg->phone);
	send(socketfd,msg,sizeof(MSG),0);
}
#endif


void register_print()
{
	printf("-----------------------------------\n");
	printf("**please input  number*************\n");
	printf("*****1.注册   2.登录   3. 退出*****\n");
	printf("***********************************\n");
	printf("***********************************\n");
	printf("********    regiseter      ********\n");
	printf("-----------------------------------\n");
}
void user_login_print()
{
	printf("-----------------------------------\n");
	printf("**你是用户，可以进行以下操作*******\n");
	printf("*****4.查看自己的个人信息**********\n");
	printf("*****5.对自己的个人信息进行修改****\n");
	printf("*****6.退出登录********************\n");
	printf("********       login        *******\n");
	printf("-----------------------------------\n");
	
}
void manager_login_print()
{
	printf("-----------------------------------\n");
	printf("**你是管理员，可以进行以下操作*****\n");
	printf("**10.新增员工信息*******************\n");
	printf("**20.删除员工信息*******************\n");
	printf("**30.改变员工信息*******************\n");
	printf("**40.查找员工信息*******************\n");
	printf("**50.退出登录***********************\n");
	printf("------------------------------------\n");
}
