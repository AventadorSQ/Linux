/*************************************************************************************************************************************
 * 实现线程池
 * 线程池类，任务类
 * **********************************************************************************************************************************/
#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<queue>
#include<pthread.h>
#include<time.h>
using namespace std;
#define MAX_THR 5
#define MAX_QUE 10

//template<class T>
typedef bool (*taskCallback)(int data);

//template<class T>
class MyTask
{
	public:
		MyTask(){}
		MyTask(int data, taskCallback callback)
			:_data(data)
			 ,_callback(callback)
	{}
		~MyTask()
		{}
		void SetTask(int data, taskCallback callback)
		{
			_data = data;
			_callback = callback;
		}
		bool Run()
		{
			return _callback(_data);
		}
	private:
		int _data;
		taskCallback _callback;
};

class ThreadPool
{
	public:
		ThreadPool(int max = MAX_THR, int cap = MAX_QUE)
			:_thr_max(max)
			 ,_capacity(cap)
	{
		_quit_flag = false;
		_thr_cur = max;
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&_cond_pro, NULL);
		pthread_cond_init(&_cond_con, NULL);
	}
		~ThreadPool()
		{
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&_cond_pro);
			pthread_cond_destroy(&_cond_con);
		}

		//static 防止this指针和arg争抢
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
			if(_quit_flag == true)
			{
				QueueUnlock();
				return false;
			}
			while(QueueIsFull())
			{
				ProductorWait();
			}
			QueuePush(task);
			ConsumerWake();
			QueueUnlock();
			return true;
		}

		void ThreadQuit()
		{
			QueueLock();
			_quit_flag = true;
			QueueUnlock();
			while(_thr_cur > 0)
			{
				ConsumerWakeAll();
				usleep(1000);
			}
		}

	private:
		void QueueLock()
		{
			pthread_mutex_lock(&mutex);
		}
		void QueueUnlock()
		{
			pthread_mutex_unlock(&mutex);
		}
		void ProductorWait()
		{
			pthread_cond_wait(&_cond_pro, &mutex);
		}
		void ProductorWake()
		{
			pthread_cond_signal(&_cond_pro);
		}
		void ConsumerWait()
		{
			//没有任务的时候，线程可以推出
			if(_quit_flag == true)
			{
				_thr_cur--;
				pthread_mutex_unlock(&mutex);
				cout<<"thread:"<<pthread_self()<<"exit!!!"<<endl;
				pthread_exit(NULL);
			}
			{
				pthread_exit(NULL);
			}
			pthread_cond_wait(&_cond_con, &mutex);
		}
		void ConsumerWake()
		{
			pthread_cond_signal(&_cond_con);
		}
		void ConsumerWakeAll()
		{
			pthread_cond_broadcast(&_cond_con);
		}
		bool QueueIsEmpty()
		{
			return _q.empty();
		}
		bool QueueIsFull()
		{
			return _q.size() == _capacity;
		}
		void QueuePop(MyTask* task)
		{
			*task = _q.front();
			_q.pop();
		}
		void QueuePush(MyTask& task)
		{
			_q.push(task);
		}
	private:
		bool _quit_flag;//线程池中线程的退出标志
		int _thr_max;//当前线程池最大的线程数量
		int _thr_cur;//线程池中当前线程数量
		queue<MyTask> _q;//任务队列
		int _capacity;
		pthread_mutex_t mutex;
		pthread_cond_t _cond_pro;
		pthread_cond_t _cond_con;
};

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
	ThreadPool p;
	p.ThreadInit();

	MyTask task[10];
	for(int i = 0; i < 10; i++)
	{
		task[i].SetTask(i, TaskHandler);
		p.AddTask(task[i]);
	}
	p.ThreadQuit();
	return 0;
}
