CXXFLAGS= -g -Wall  -std=c++0x
TARGET= Test_Pool
OBJS= MyThread.o ThreadPool.o Task.o Test_Pool.o
$(TARGET):$(OBJS)
	g++  -o $(TARGET) $(OBJS) -lpthread
MyThread.o:MyThreadPool.h
ThreadPool.o:MyThreadPool.h
Task.o:MyThreadPool.h
Test_Pool.o:MyThreadPool.h

.PHONY: clean
clean:
	rm -f *.o *.gch

