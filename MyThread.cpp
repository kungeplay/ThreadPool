#include <string.h>
#include <memory>
#include "MyThreadPool.h"

using namespace std;

int MyThread::threadNum=0;
MyThread::MyThread(ThreadPool* pool):pool(pool)
{

	int err=pthread_create(&tid,NULL,threadrun,this);
	if(err!=0)
	{
		throw runtime_error(strerror(err));
	}
	++threadNum;
	threadid=threadNum;

}
void *MyThread::threadrun(void *arg)
{
	MyThread *const pthis=reinterpret_cast<MyThread *>(arg);
	while(true)
	{
		int signop=0;
		sigwait(&threadset,&signop);
		pthis->task->run();


		pthread_mutex_lock(&(pthis->pool->mutex));

		pthis->pool->BusyThreadList.remove(pthis);
		pthis->pool->IdleThreadList.push(pthis);

		pthread_mutex_unlock(&(pthis->pool->mutex));

		pthread_cond_signal(&(pthis->pool->cond));

	}
}
