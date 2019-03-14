/*写一个死循环程序，查看进程信息
 * */
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
int main()
{
    //pid_t getpid(void);
    //获取调用进程的pid
    //pid_t fork(void)
    //通过复制调用进程，创建一个子进程
    pid_t pid = fork();
    //因为父子进程返回值不同，父子代码虽然相同，但是父进程因为返回会进入不同的判断执行体
    if(pid == 0)
    {
    printf("I am child!!\n");
    }
    else if(pid > 0)
    {
    printf("Parent\n");
    }
    else
    {
    //打印上一个系统调用的错误原因
    perror("fork error");
    }
    printf("pid: %d\n",getpid());
    while(1)
    {
	sleep(2);
	printf("------\n");
    }
    return 0;
}
