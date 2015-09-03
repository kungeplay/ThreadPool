#ifndef MYTHREADPOOL_H
#define MYTHREADPOOL_H

#include <iostream>
#include <deque>
#include <stack>
#include <list>

extern "C"
{
	#include <pthread.h>
	#include <signal.h>
}
using namespace std;

class MyTask;
class MyThread;
class ThreadPool;

extern sigset_t threadset;	//变量一般不要定义在.h文件中。

class ThreadPool
{
public:
	explicit ThreadPool(int n=10);
	~ThreadPool();
	bool add_task(MyTask *task);
	void run();
	int get_thread_num(){return currentCount;}
private:
	void init_pool();
	void do_task(MyThread * thread,MyTask * task);
	friend class MyThread;
private:

	deque<MyTask*> TaskList;
	stack<MyThread*> IdleThreadList;
	list<MyThread*> BusyThreadList;	
	int currentCount;
	pthread_mutex_t  mutex;
	pthread_cond_t  cond;

};

class MyThread
{
public:
	explicit MyThread(ThreadPool * pool);
	~MyThread(){};
	static void *threadrun(void *);//线程开始执行的函数在类中必须为静态
	pthread_t get_pthread(){return tid;}
	int get_id(){return threadid;}
	void modify_task(MyTask *task){this->task=task;}
private:
	ThreadPool *pool;
	pthread_t tid;
	MyTask * task;
	int threadid;
	static int threadNum;
};

class MyTask
{
public:
	MyTask(){++taskNum;taskid=taskNum;}
	virtual ~MyTask(){}
	virtual void run()=0;
	int get_id(){return taskid;}
private:
	int taskid;
	static int taskNum;
};
#endif
