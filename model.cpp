/**************************************************************************************************************************************
 * 实现生产者与消费者模型
 * ***********************************************************************************************************************************/

#include<iostream>
#include<string>
#include<queue>
using namespace std;
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
class BlockQueue
{
	public:
		BlockQueue(int cap = 10)
			:capacity(cap)
		{
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&productor, NULL);
			pthread_cond_init(&customer, NULL);
		}
		~BlockQueue()
		{
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&productor);
			pthread_cond_destroy(&customer);
		}
		void QueuePop(int* data)
		{
			QueueLock();
			while(QueueIsEmpty())
			{
				CustomerWait();
			}
			*data = q.front();
			q.pop();
			QueueUnlock();
			ProductorSignal();
		}
		void QueuePush(int data)
		{
			QueueLock();
			while(QueueIsFull())
			{
				ProductorWait();
			}
			q.push(data);
			QueueUnlock();
			CustomerSignal();
		}
	private:
		//互斥锁
		void QueueLock()
		{
			pthread_mutex_lock(&mutex);
		}
		void QueueUnlock()
		{
			pthread_mutex_unlock(&mutex);
		}
		//消费者等待与唤醒
		void CustomerWait()
		{
			pthread_cond_wait(&customer, &mutex);
		}
		void CustomerSignal()
		{
			pthread_cond_signal(&customer);
		}
		//生产者等待与唤醒
		void ProductorWait()
		{
			pthread_cond_wait(&productor, &mutex);
		}
		void ProductorSignal()
		{
			pthread_cond_signal(&productor);
		}
		//判断数据是否为空或者已满
		bool QueueIsEmpty()
		{
			return q.empty();
		}
		bool QueueIsFull()
		{
			return q.size() == capacity;
		}
	private:
		queue<int> q;
		//控制队列的大小
		int capacity;
		//保证线程的安全，唯一访问
		pthread_mutex_t mutex;
		//保证取放数据的同步---唤醒与等待
		pthread_cond_t productor;
		pthread_cond_t customer;

};

//*******************************************************
//尝试加锁
pthread_mutex_t _mutex;
BlockQueue q;
int dataS = 0;

void* customer(void* arg)
{
	//BlockQueue* q = (BlockQueue*)arg;
	int tmp = reinterpret_cast<long long>(arg);
	while(1)
	{
		int data;
		//pthread_mutex_lock(&_mutex);
		q.QueuePop(&data);
		//pthread_mutex_unlock(&_mutex);
		//cout<< "--------("<< (int)arg <<")号消费者拿到数据:" << data <<endl;
		printf("~~~~~(%d)号消费者拿到数据:%d\n", tmp, data);
		//usleep(500000);
	}
	return NULL;
}

void* productor(void* arg)
{
	//BlockQueue* q = (BlockQueue*)arg;
	int tmp = reinterpret_cast<long long>(arg);
	while(1)
	{
		//pthread_mutex_lock(&_mutex);
		q.QueuePush(dataS);
		dataS++;
		//pthread_mutex_unlock(&_mutex);
		//cout<<"~~~("<< (int)arg <<")号生产者放入数据:"<<i<<endl;
		printf("~~~(%d)号生产者放入数据:%d\n", tmp, dataS);
		//usleep(500000);
	}
	return NULL;
}


int main()
{
	//BlockQueue q;
	pthread_t ProductorTid[3];
	pthread_t CustomersTid[6];
	pthread_mutex_init(&_mutex, NULL);
	for(int i = 0; i < 3; i++)
	{
		void * tmp = reinterpret_cast<void *>(i);
		if(pthread_create(&ProductorTid[i], NULL, productor, tmp))
		{
			cout<<"create Productor thread error"<<endl;
			return -1;
		}
	}

	for(int i = 0; i < 6; i++)
	{
		void * tmp = reinterpret_cast<void *>(i);
		if(pthread_create(&CustomersTid[i], NULL, customer, tmp))
		{
			cout<<"create thread error"<<endl;
			return -1;
		}
	}

	for(int i = 0; i < 3; i++)
	{
		pthread_join(ProductorTid[i], NULL);
	}

	for(int i = 0; i < 6; i++)
	{
		pthread_join(CustomersTid[i], NULL);
	}

	pthread_mutex_destroy(&_mutex);
	return 0;
}
