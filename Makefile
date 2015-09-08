SRC_FILES=MyThread.cpp ThreadPool.cpp Task.cpp Test_Pool.cpp
OBJ_FILES=$(patsubst %.cpp,%.o, ${SRC_FILES})
DEP_FILES=$(patsubst %.cpp,%.dep, ${SRC_FILES})

VPATH=./
CXXFLAGS=-c -g -std=c++0x
LDFLAGS=-g -lpthread


TARGET= Test_Pool
${TARGET}:${OBJ_FILES}
	g++ ${LDFLAGS} -o ${TARGET} ${OBJ_FILES}

%.o : %.cpp
	g++ ${CXXFLAGS} -o $@  $^

clean:
	rm *.o ${TARGET}

#include ${DEP_FILES}
#
#%.dep:%.cpp
#	set -e;rm -f $@;\
#	g++ -MM $(CXXFLAGS) $< > $@.$$$$;\
#	sed 's,\($*\)\.o[ :]*, \1.o $@: ,g' < $@.$$$$ > $@;\
#	rm -f $@.$$$$
