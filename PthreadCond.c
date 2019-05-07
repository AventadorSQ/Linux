/**************************************************************************************************************************************
 * 条件变量的基本操作与认识
 * ***********************************************************************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>

int Is_have_noodles = 0;
pthread_t tid1, tid2;
//条件变量
//pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond;
pthread_mutex_t mutex;

void* thr_sale(void* arg)
{
	pthread_detach(pthread_self());
	while(1)
	{
		//  因为Is_have_noodle是临界资源所以需要保护，因此要加锁
    	pthread_mutex_lock(&mutex);
		//如果有面，没人吃则等待
		while(Is_have_noodles != 0)
		{
			//int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex);
			//  cond: 条件变量
			//  mutex: 互斥锁
			//
			//  wait中集合了解锁和休眠+被唤醒后加锁的原子操作
			//  如果不能做面就要解锁让别人能吃面
			pthread_cond_wait(&cond, &mutex);
			//sleep(1);
		}
		Is_have_noodles++;
		printf("sale----make noodles!!!\n");
		//int pthread_cond_broadcast(pthread_cond_t *cond);
		//唤醒所有等待的线程
		//int pthread_cond_signal(pthread_cond_t *cond);
		//至少唤醒一个等待的线程
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* thr_buy(void* arg)
{
	pthread_detach(pthread_self());
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(Is_have_noodles != 0)
		{
			printf("buy----~~~~~~~~delicious!!!\n");
			Is_have_noodles--;
			if(Is_have_noodles == 0)
			{
				pthread_cond_signal(&cond);
			}
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			//没有面则休眠
			pthread_cond_wait(&cond, &mutex);
	    	//sleep(1);
			//pthread_mutex_unlock()
			//pause();
			//pthread_mutex_lock()
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}


int main()
{

	int ret;
	//1.初始化条件变量
	// int pthread_cond_init(pthread_cond_t *restrict cond,const pthread_condattr_t *restrict attr);
	// cond : 条件变量
	// attr : 条件变量属性，通常置NULL
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);
	ret = pthread_create(&tid1, NULL, thr_sale, NULL);
	if(ret != 0)
	{
		printf("thread create error\n");
		return -1;
	}

	ret = pthread_create(&tid2, NULL, thr_buy, NULL);
	if(ret != 0)
	{
		printf("thread create error\n");
		return -1;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	//销毁条件变量
	//int pthread_cond_destroy(pthread_cond_t *cond);
	// cond : 条件变量
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
	return 0;
}
