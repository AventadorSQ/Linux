/*
 * 编写一个简易得shell，实现能够运行shell的外部命令
 *    1.读取缓冲区
 *    2.解析输入
 *    3.创建子进程
 *    4.程序替换
 */

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int main()
{
	//读取数据
  while(1)
  {
     char buf[1024] = {0};
     printf("[shangqi@localhost ~]$");
     fflush(stdout);//刷新标准输出缓冲区---将缓冲区的数据直接打印
     memset(buf, 0x00, 1024);
	 //%[^\n] scanf取数据的时候遇到各种空白字符就会停止读取，为了读取所有的输入。让scanf遇到换行的时候再终止
	 //scanf返回值读取数据的个数，如果读取失败，避免缓冲区中的换行取不出来，导致死循环（防备直接回车情况）
     if(scanf("%[^\n]%*c",buf) != 1)
	 {
		 scanf("%*c");
		 continue;
	 }
	 
	 //解析字符
	 char* ptr= buf;
	 char* argv[32];
	 int argc = 0;
	 while(*ptr!= '\0')
	 {
		 if(!isspace(*ptr))
		 {
			 argv[argc++] = ptr;
			 while(*ptr != '\0' && !isspace(*ptr))
			 {
				 ptr++;
			 }
		 }
         *ptr='\0';
		 ptr++;
	 }
	 //参数结尾必须为空
	 argv[argc] = NULL;

	 //创建子进程
	 int pid = fork();
	 if(pid < 0)
	 {
		 perror("fork error");
		 return -1;
	 }
	 else if(pid == 0)
	 {
		 //子进程程序替换运行指定命令
		 execvp(argv[0], argv);
		 exit(0);
	 }
	 //避免僵尸子进程
	 waitpid(pid, NULL, 0);
	 //int i = 0;
	 //while(i<argc)
	 //{
	//	 printf("argv[%d] = [%s]\n", i, argv[i]);
	//	 i++;
	 //}
  }

  return 0;
}
