/**********************************************
 * 进程间通信之消息队列
 * *******************************************/

#include<sys/types.h>
#include<sys/msg.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>


struct Msg
{
	long msg_type;
	char msg_text[1024];
};


void sigcb(int no)
{
	while(waitpid(-1, NULL, WNOHANG)>0);
}
int main()
{
	//signal(SIGCHLD, sigcb);
	pid_t pid;

	key_t key1;
	key1 = ftok("./file.txt", 1);
	if(key1 == -1)
	{
		printf("生成键值失败!!!\n");
	}

	//1.创建消息队列
	//msget();
	//int msgget(key_t key, int msgflg);
	//msgflg:标识与权限组合
	//       IPC_CREAT 存在则直接使用，否则创建新的消息队列
	//       0         获取存在的消息队列
	//成功 返回标识符 失败：-1

	//2.像消息队列写入数据
	//int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
	//msgqid 消息队列标识符
	//msgp   消息结构体指针
	//msgsz  消息内容长度
	//msgflg 可选标志
	//    0：         消息队列已满则阻塞
	//    IPC_NOWAIT：如果满了不阻塞直接返回
	//成功：0 失败：-1
	//printf("请输入要写入的数据:");
	//scanf("%s", msg.msg_text);

	pid = fork();
	int QueueFlag;
	//让父进程写入，子进程读取·
	if(pid > 0)
	{
		QueueFlag = msgget(key1, IPC_CREAT|0664);
		if(QueueFlag == -1)
		{
			printf("创建消息队列失败!!!\n");
			return 0;
		}
		printf("parent open msg success: %d\n", QueueFlag);
		struct Msg msg;
		int i;
		//strcpy(msg.msg_text, "nihao");
	    printf("请输入要写入的数据:");
    	scanf("%s", msg.msg_text);
	    msg.msg_type = 2;
		i = msgsnd(QueueFlag, (struct Msg*)&msg, strlen(msg.msg_text),0);
		if(i == -1)
		{
			printf("写入数据失败\n");
			return 0;
		}
		wait(NULL);
	//删除队列
	//int msgctl(int msqid, int cmd, struct msqid_ds *buf);
	//msqid：消息队列标识符
	//cmd：  控制命令
	//  IPC_RMID: 删除消息队列
	//  IPC_STAT：获取msqid_ds结构，保存早buf中
	//  IPC_SET： 设置msqid_ds，按照buf指向的结构体的值
	//buf：  指针
	//成功 0 失败：-1
	
	int num;
	num = msgctl(QueueFlag, IPC_RMID, NULL);
	if(num == -1)
	{
		printf("删除消息队列失败\n");
		return 0;
	}
	printf("删除消息队列成功!\n");
	
	return 0;
	}
	else if(pid == 0)
	{
	//3.读取数据并移除
	//ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
	//msqid  消息队列标识符
	//msgp   消息队列结构体指针
	//msgsz  要读取的数据长度
	//msgtyp 读取的数据类型
	//msgflg 接收消息的可选标志
	//   0：         队列为空，阻塞
	//   IPC_NOWAIT：队列为空则立即返回-1
	//   MSG_NOERROR：若消息长度大于接受长度，截断返回，否则报错返回
	//   成功：接受字节数 失败：-1
		//int QueueFlag;
		QueueFlag = msgget(key1, IPC_CREAT|0664);
		if(QueueFlag == -1)
		{
			printf("创建消息队列失败!!!\n");
			return 0;
		}
		struct Msg msg;
		ssize_t size;
		size = msgrcv(QueueFlag, (struct Msg*)&msg, sizeof(msg.msg_text), 2, 0);
		printf("child open msg success: %d\n", QueueFlag);
		if(size == -1)
		{
			printf("读取数据失败\n");
			return 0;
		}
		printf("接受到[%d]个字节，数据：%s\n", size, msg.msg_text);
		
	}
else
	{
		printf("创建子进程失败\n");
		return 0;
	}
}
