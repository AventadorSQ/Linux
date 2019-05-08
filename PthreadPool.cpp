/*************************************************************************************************************************************
 * 实现线程池
 * 线程池类，任务类
 * **********************************************************************************************************************************/
#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<queue>
using namespace std;


template<class T>
typedef bool (*taskCallback)(T data);

template<class T>
class MyTask
{
	public:
		MyTask(){}
		MyTask(T data, taskCallback callback)
			:_data(data)
			 ,_callback(callback)
    	{}
		~MyTask()
		{}
		void SetTask(T data, taskCallback callback)
		{
			_data = data;
			_callback = callback;
		}
		bool Run()
		{
			return _callback(_data);
		}
	private:
		T data;
		taskCallback _callback;
};

class ThreadPool
{
	public:
		ThreadPool(int max, int cap)
			:_thr_max(max)
			 ,_capacity(cap)
	{
		_thr_cur = cap;
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_cond_pro, NULL);
		pthread_cond_init(&_cond_con, NULL);
	}
		~ThreadPool()
		{
			pthread_mutex_destroy(&_mutex);
			pthread_cond_destroy(&_cond_pro);
			pthread_cond_destroy(&_cond_con);
		}

		static void* ThreadStart(void* arg)
		{
			ThreadPool* p = (ThreadPool*)arg;

			while(1)
			{
				p->QueueLock();
				while(p->QueueIsEmpty())
				{
					p->ConsumerWait();
				}
				MyTask task;
				p->QueuePop(&task);
				p->ProductorWake();
				p->QueueUnlock();
				task.Run();
			}
			return NULL;
		}
		bool ThreadInit()
		{
			int ret;
			pthread_t tid;
			for(int i = 0; i < _thr_max; i++)
			{
				ret = pthread_create(&tid, NULL, ThreadStart, (void*)this);
				if(ret != 0)
				{
					cout<< "thread create error!!!" <<endl;
					return false;
				}
				pthread_detach(tid);
			}
			return true;
		}
		bool AddTask(MyTask& task)
		{
			QueueLock();
			while(QueueIsFull())
			{
				ProductorWait();
			}
			QueuePush(task);
			ConsumerWake();
			QueueUnlock();
			return true;
		}

	private:
		void QueueLock()
		{
			
		}
		void QueueUnlock();
		void ProductorWait();
		void ProductorWake();
		void ConsumerWait();
		void ConsumerWake();
		bool QueueIsEmpty();
		bool QueueIsFull();
		void QueuePop(MyTask* task);
		void QueuePush(MyTask& task);
	private:
		int _thr_max;
		int _thr_cur;
		queue<MyTask> _q;
		int _capacity;
		pthread_mutex_t mutex;
		pthread_cond_t _cond_pro;
		pthread_cond_t _cond_con;
}

bool TaskHandler(int data)
{
	srand(time(NULL));
	int n = rand()%5;
	cout<<"thread:"<<pthread_self()<<"is sleep" <<n<<"seconds"<<endl;
	sleep(n);
	return true;
}

int main()
{
	PthreadPool p(5, 10);
	p.PthreadInit();

	MyTask task[10];
	for(int i = 0; i < 10; i++)
	{
		task[i].SetTask(i, TaskHandler);
		p.AddTask(task[i]);
	}
	while(1)
	{
		sleep(1);
	}
	return 0;
}
