/**************************************************************************************************************************************
 * 买票程序，体验线程安全问题
 * 互斥锁的基本使用
 * ***********************************************************************************************************************************/
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

//初始化互斥锁，一定要在创建线程之前
pthread_mutex_t mutex;

int ticket_count = 100;
void* thr_start(void* arg)
{
	while(1)
	{
		// int pthread_mutex_lock(pthread_mutex_t *mutex);
		// 阻塞加锁
		// int pthread_mutex_trylock(pthread_mutex_t *mutex);
		// 非阻塞加锁
		usleep(1000);
		pthread_mutex_lock(&mutex);
		if(ticket_count > 0)
		{
			printf("yello bull---%d ~~~ get ticket:%d\n",(int)arg, ticket_count--);
			// 解锁
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			//printf("no have tickets, exit!\n");
			//加锁之后，在任意可能退出线程的地方都要解锁解锁
			pthread_mutex_unlock(&mutex);
			return NULL;
		}
		//解锁
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main()
{
	pthread_t tid[4];
	int i = 0;
	int ret = 0;

	//1.  初始化互斥锁
	// int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
	// mutex 互斥锁变量
	// 互斥锁属性，通常置NULL
	pthread_mutex_init(&mutex, NULL);

	for(;i < 4; i++)
	{
		ret = pthread_create(&tid[i], NULL, thr_start, (void*)i);
		if(ret != 0)
		{
			printf("thread create error\n");
			return -1;
		}
	}

	for(i = 0; i < 4; i++)
	{
		pthread_join(tid[i], NULL);
	}
	//4.  销毁互斥锁
	//int pthread_mutex_destroy(pthread_mutex_t *mutex);
	//  mutex: 互斥锁变量
	pthread_mutex_destroy(&mutex);

	return 0;
}
