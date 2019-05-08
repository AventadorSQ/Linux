/**************************************************************************************************************************************
 * posix标准信号量的使用
 * 使用信号量实现一个线程安全的队列
 * **********************************************************************************************************************************/

#include<iostream>
#include<vector>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
using namespace std;
#include<semaphore.h>

class RingQueue
{
	public:
		RingQueue(int cap = 10)
			:capacity(cap)
			 ,q(10)
	{
		ProductorStep = 0;
		CustomerStep = 0;
		sem_init(&SemMutex, 0, 1);
		// int sem_init(sem_t *sem, int pshared, unsigned int value);
		// 信号量的初始化
		// sem: 信号量
		// pshared： 
		//       0    用于线程间同步与互斥
		//       !0   用于进程间同步与互斥
		// value:   信号量的计数初值
		sem_init(&IdleSpace, 0, capacity);
		sem_init(&DataSpace, 0, 0);
	}
		~RingQueue()
		{
			sem_destroy(&SemMutex);
			// int sem_destroy(sem_t *sem);
			// 销毁信号量
			sem_destroy(&IdleSpace);
			sem_destroy(&DataSpace);
		}
		void QueuePush(int data)
		{
			ProductorWait();
			QueueLock();
			q[ProductorStep] = data;
			ProductorStep++;
			ProductorStep %= capacity;
			QueueUnlock();
			CustomerSignal();
		}
		void QueuePop(int* data)
		{
			CustomerWait();
			QueueLock();
			*data = q[CustomerStep];
			CustomerStep++;
			CustomerStep %= capacity;
			QueueUnlock();
			ProductorSignal();
		}
	private:
		void QueueLock()
		{
			sem_wait(&SemMutex);
		}
		void QueueUnlock()
		{
			sem_post(&SemMutex);
		}
		void CustomerWait()
		{
			//int sem_wait(sem_t *sem);
			//计数判断，若<=0;则阻塞等待
			//int sem_trywait(sem_t *sem);
			//计数判断，若<=0;则报错返回
			//int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
			//计数判断,若<=0;则现时等待
			sem_wait(&DataSpace);
		}
		void CustomerSignal()
		{
			// int sem_post(sem_t *sem);
			// 唤醒sem信号量等待队列上的线程
			sem_post(&DataSpace);
		}
		void ProductorWait()
		{
			sem_wait(&IdleSpace);
		}
		void ProductorSignal()
		{
			sem_post(&IdleSpace);
		}
	private:
		vector<int> q;
		int capacity;
		int CustomerStep;
		int ProductorStep;
		sem_t SemMutex;  //信号量实现互斥
		sem_t IdleSpace; //空闲空间
		sem_t DataSpace; //数据个数
};

//环形队列
RingQueue q;
//放入的数据
int dataS = 0;
//取出的数据
int data;

void* ProductorStart(void* arg)
{
	int temp = reinterpret_cast<long>(arg);
	while(1)
	{
		q.QueuePush(dataS);
		cout<< "(" << temp <<")号放入数据~~~~" << dataS <<endl;
		dataS++;
	}
	return NULL;
}

void* CustomerSStart(void* arg)
{
	int temp = reinterpret_cast<long>(arg);
	while(1)
	{
		q.QueuePop(&data);
		cout<< "~~~~~~(" << temp <<")号顾客get数据~~~~" << data <<endl;
	}
	return NULL;
}

int main()
{
	int ret;
	pthread_t ProductorTid[3];
	pthread_t CustomerSTid[6];
	for(int i = 0; i < 3; i++)
	{
		void* temp = reinterpret_cast<void*>(i);
		ret = pthread_create(&ProductorTid[i], NULL, ProductorStart, temp);
		if(ret != 0)
		{
			cout<<"create productor thread error"<<endl;
			return -1;
		}
	}

	for(int i = 0; i < 6; i++)
	{
		void* temp = reinterpret_cast<void*>(i);
		ret = pthread_create(&CustomerSTid[i], NULL, CustomerSStart, temp);
		if(ret != 0)
		{
			cout<<"create customer thread error"<<endl;
			return -1;
		}
	}

	for(int i = 0; i < 3; i++)
	{
		pthread_join(ProductorTid[i], NULL);
	}
	for(int i = 0; i < 6; i++)
	{
		pthread_join(CustomerSTid[i], NULL);
	}

	return 0;
}
