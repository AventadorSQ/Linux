#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

void *PthreadFunc(void* arg)
{
	sleep(2);
	return "SSS";
}

int main()
{
	pthread_t tid;
	int val = pthread_create(&tid, NULL, PthreadFunc, NULL);
	if(val != 0)
	{
		printf("pthread create error\n");
		return -1;
	}

	//int pthread_join(pthread_t thread, void **retval);
	//  阻塞等待线程退出
	//  thread: 等待的线程ID
	//  retval：用户获取线程退出的返回值
    //char* retval;
	//void* retval = NULL;
	//pthread_join(tid, &retval);
	//printf("retval:%s\n", retval);
	while(1)
	{
		sleep(1);
		printf("join~~~~\n");
	}
	return 0;
}
