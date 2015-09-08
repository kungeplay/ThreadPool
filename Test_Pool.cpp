#include <iostream>
extern "C"
{
	#include <unistd.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <string.h>
	#include <errno.h>
	#include <time.h>
}
#include "MyThreadPool.h"

using namespace std;

class Task:public MyTask
{
public:
	void run()
	{
		sleep(5);
	}
};

string srcfile="/mnt/Windows/E/云图.rmvb";
string desfile="/home/Jiakun/视频/yuntu.rmvb";
const int MAXLINE=4096;

class CopyTask:public MyTask
{
private:
	long long beginbyte;
	long long lenbyte;

public:
	CopyTask(long long m,long long n):beginbyte(m),lenbyte(n){}
	void run()
	{
		int fd_in;
		int fd_out;
		int bytes_in=0;
		int bytes_out=0;
		char buf[MAXLINE];
		if(((fd_in=open(srcfile.c_str(),O_RDONLY))!=-1)&&(lseek(fd_in,beginbyte,SEEK_SET)!=-1))
		{
			if(((fd_out=open(desfile.c_str(),O_WRONLY))!=-1)&&(lseek(fd_out,beginbyte,SEEK_SET)!=-1))
			{
				while(lenbyte>=MAXLINE&&(bytes_in=read(fd_in,buf,MAXLINE))>0)
				{
					bytes_out=write(fd_out,buf,bytes_in);
					if(bytes_out!=bytes_in)
					{
						cerr<<"copy error!"<<endl;
						return;
					}
					lenbyte-=bytes_in;
				}
				if(lenbyte>0&&lenbyte<MAXLINE)
				{
					while(lenbyte>0&&(bytes_in=read(fd_in,buf,lenbyte))>0)
					{
						bytes_out=write(fd_out,buf,bytes_in);
						if(bytes_out!=bytes_in)
						{
							cerr<<"copy error"<<endl;
							return;
						}
						lenbyte-=bytes_in;
					}
				}

				close(fd_out);
			}
			else
			{
				cerr<<"open or lseek desfile failed:"<<strerror(errno)<<endl;
			}
			close(fd_in);			
		}
		else
		{
			cerr<<"open or lseek srcfile failed:"<<strerror(errno)<<endl;
		}
	}
};

int main()
{
	ThreadPool pool(2);

//	Task t1,t2,t3,t4,t5,t6,t7,t8;
//	pool.add_task(&t1);
//	pool.add_task(&t2);
//	pool.add_task(&t3);
//	pool.add_task(&t4);
//	pool.add_task(&t5);
//	pool.add_task(&t6);
//	pool.add_task(&t7);
//	pool.add_task(&t8);
//	pool.run();

	struct stat buf;
	if(stat(srcfile.c_str(),&buf)==0)
	{
		cout<<"文件大小(M):"<<buf.st_size/1024/1024<<endl;
		long long srclen=buf.st_size;
		mode_t mode=S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP;
		if(open(desfile.c_str(),O_WRONLY|O_CREAT|O_TRUNC,mode)!=-1)
		{
			if(truncate(desfile.c_str(),srclen)!=-1)
			{
				long long beginposit=0;
				long long copybytes=1024*1024*80;
				while(srclen>0)
				{
					if(srclen>copybytes)
					{
						CopyTask *task=new CopyTask(beginposit,copybytes);
						pool.add_task(task); 
						beginposit+=copybytes;
						srclen-=copybytes;
					}
					else if(srclen>0)
					{
						CopyTask *task=new CopyTask(beginposit,srclen);
						pool.add_task(task);
						break;
					}
				}
				time_t time1,time2;
				time(&time1);
				pool.run();
				time(&time2);
				cout<<"耗时(秒):"<<time2-time1<<endl;
			}
			else
			{
				cerr<<"truncate failed: "<<strerror(errno)<<endl;
			}

		}
		else
		{
			cerr<<"open desfile failed: "<<strerror(errno)<<endl;
		}
	}
	else
	{
		cerr<<"stat src file error"<<endl;
	}

}
