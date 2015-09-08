#include "MyThreadPool.h" 
using namespace std;

sigset_t threadset;

void ThreadPool::init_pool()
{
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
	sigemptyset(&threadset);
	sigaddset(&threadset,SIGUSR1);
	pthread_sigmask(SIG_BLOCK,&threadset,NULL);	
}
ThreadPool::ThreadPool(int n):currentCount(0)
{
	init_pool();
	for(int i=0;i<n;++i)
	{
		MyThread* thread=NULL; 
		try
		{
			thread=new MyThread(this);
		}
		catch (exception &ex)
		{	
			cerr<<"创建线程失败:"<<ex.what()<<endl;
			delete thread;
			continue;
		}
		++currentCount;	
		cout<<"创建线程"<<thread->get_id()<<endl;
		IdleThreadList.push(thread);
	}
}

ThreadPool::~ThreadPool()
{
	while(!IdleThreadList.empty())
	{
		delete IdleThreadList.top();
		IdleThreadList.pop();
	}
}

bool ThreadPool::add_task(MyTask *task)
{
	TaskList.push_back(task);
	return true;
}

void ThreadPool::run()
{
	while(!TaskList.empty())
	{
		pthread_mutex_lock(&mutex);

		while(IdleThreadList.empty())
			pthread_cond_wait(&cond,&mutex);

		MyThread * thread=IdleThreadList.top();
		IdleThreadList.pop();

		BusyThreadList.push_back(thread);

		MyTask *task=TaskList.front();
		TaskList.pop_front();
		cout<<"线程"<<thread->get_id()<<" 执行任务"<<task->get_id()<<endl;
		do_task(thread,task);

		pthread_mutex_unlock(&mutex);
	}

	pthread_mutex_lock(&mutex);
	while(!BusyThreadList.empty())
		pthread_cond_wait(&cond,&mutex);
	pthread_mutex_unlock(&mutex);
	cout<<"所有任务执行完毕!"<<endl;

}

void ThreadPool::do_task(MyThread * thread,MyTask *task)
{
	thread->modify_task(task);
	pthread_t tid=thread->get_pthread();

	pthread_kill(tid,SIGUSR1);
}

