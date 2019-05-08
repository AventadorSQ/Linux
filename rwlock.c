/**************************************************************************************************************************************
 * 读写锁的使用与体会-----读者写者问题
 * ***********************************************************************************************************************************/

#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

pthread_rwlock_t rwlock;

void* WriterStart(void* arg)
{
	while(1)
	{
		//写者加写锁
		//int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
		// 非阻塞加读写锁
		//int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
		// 阻塞加读写锁
		pthread_rwlock_wrlock(&rwlock);
		printf("(%d)号写者写入~~~~~~~~~~~~~~~hehe\n",(int)arg);
		usleep(1000);
		//int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
		//解锁，解锁不管读锁和写锁
		pthread_rwlock_unlock(&rwlock);
		usleep(10000);
	}
	return NULL;
}

void* ReaderStart(void* arg)
{
	while(1)
	{
		//int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
		//  阻塞加读锁
		//int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
		//  非阻塞加读锁
		pthread_rwlock_rdlock(&rwlock);
		printf("----(%d)号读者读~~~haha\n",(int)arg);
		usleep(1000);
		pthread_rwlock_unlock(&rwlock);
	}
	return NULL;
}
int main()
{
	int i, ret;
	pthread_t WriterTid[2];
	pthread_t ReaderTid[6];
	//因为读写锁默认是读锁优先；因此需要设置优先级属性
	//int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
	//  读写锁属性的销毁
	//int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
	//  读写锁属性的初始化
	//int pthread_rwlockattr_setkind_np(pthread_rwlockattr_t* attr, int pref)
	//  设置读写锁优先级
	//  attr：读写锁
	//  pref：优先级
	//      PTHREAD_RWLOCK_PREFER_WRITER_NP 写着优先有BUG，导致行为和读者优先一致
	//      PTHREAD_RWLOCK_PREFER_READER_NP
	//      读者优先
	//      PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP
	//      写者优先，但写者不能递归加锁
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
	//初始化读写锁
	//int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,const pthread_rwlockattr_t *restrict attr);
	//  rwlock: 读写锁
	//  attr:   读写锁属性
	//pthread_rwlock_init(&rwlock, NULL);
	pthread_rwlock_init(&rwlock, &attr);


	for(i = 0; i < 2; i++)
	{
		ret = pthread_create(&WriterTid[i], NULL, WriterStart, i);
		if(ret != 0)
		{
			printf("thread create error!!!\n");
			return -1;
		}
	}

	for(i = 0; i < 6; i++)
	{
		ret = pthread_create(&ReaderTid[i], NULL, ReaderStart, i);
		if(ret != 0)
		{
			printf("thread create error!!!\n");
			return -1;
		}
	}

	for(i = 0; i < 2; i++)
	{
		pthread_join(WriterTid[i], NULL);
	}

	for(i = 0; i < 6; i++)
	{
		pthread_join(ReaderTid[i], NULL);
	}

	//销毁读写锁属性变量
	pthread_rwlockattr_destroy(&attr);
	//4.销毁读写锁
	//int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
	// rwlock : 读写锁
	pthread_rwlock_destroy(&rwlock);

	return 0;
}
