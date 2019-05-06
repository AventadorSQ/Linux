#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

void* pthread_start(void* arg)
{
	// pthread_t pthread_self(void);
	// 返回调用线程的线程ID
	pthread_t tid = pthread_self();
	while(1)
	{
    	printf("second_thread_haha~~~~~~~~~~~%d---tid:%p\n", arg, tid);
		sleep(1);
		//exit(0);exit退出的是整个进程（所有线程都会退出）
		// 进程退出返回值（一个字节）：说明任务完成情况

		//void pthread_exit(void *retval);
		// 退出调用线程，并且返回retval
		// 线程的返回值，不仅可以判断终止场景，并且可以获取任务处理结果
		//pthread_exit("PTHREADEXIT");
	}
	return NULL;
}

int main(int argc, int* argv[])
{
	//int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
	//thread: 用于保存返回的线程ID
	//attr:   线程属性，通常置NULL
	//start_routine: 线程入口函数
	//arg: 给线程传入的参数
	//返回值: 成功 0 ， 失败 : errno
	pthread_t tid;
	int val;
	val = pthread_create(&tid, NULL, pthread_start, (void*)666);
	if(val != 0)
	{
		printf("pthread create error\n");
		return -1;
	}

	sleep(1);
	//int pthread_cancel(pthread_t thread);
	//  取消指定线程---让指定线程退出
	//  thread:线程id
	//pthread_cancel(pthread_self());
	pthread_cancel(tid);

	while(1)
	{
		printf("Main_pthread_hehe~~~Second tid:%p---Main tid:%d\n",tid, getpid());
		sleep(1);
		//pthread_exit(NULL);
		//在main函数中return是退出进程
		//在线程入口函数中return是退出线程
	}
}

