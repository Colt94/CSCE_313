# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c Histogram.cpp

FIFOreqchannel.o: FIFOreqchannel.h FIFOreqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c FIFOreqchannel.cpp

KernelSemaphore.o: KernelSemaphore.h KernelSemaphore.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c KernelSemaphore.cpp

SHMBoundedBuffer.o: SHMBoundedBuffer.h SHMBoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c SHMBoundedBuffer.cpp

MQreqchannel.o: MQreqchannel.h MQreqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c MQreqchannel.cpp

SHMreqchannel.o: SHMreqchannel.h SHMreqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c SHMreqchannel.cpp

dataserver: dataserver.cpp reqchannel.o FIFOreqchannel.o MQreqchannel.o KernelSemaphore.o SHMBoundedBuffer.o SHMreqchannel.o
	g++ -g -w -Wall -O1 -std=c++11 -o dataserver dataserver.cpp reqchannel.o FIFOreqchannel.o MQreqchannel.o KernelSemaphore.o SHMBoundedBuffer.o SHMreqchannel.o -lpthread -lrt

client: client.cpp reqchannel.o FIFOreqchannel.o MQreqchannel.o BoundedBuffer.o Histogram.o KernelSemaphore.o SHMBoundedBuffer.o SHMreqchannel.o
	g++ -g -w -Wall -O1 -std=c++11 -o client client.cpp reqchannel.o FIFOreqchannel.o MQreqchannel.o BoundedBuffer.o Histogram.o KernelSemaphore.o SHMBoundedBuffer.o SHMreqchannel.o -lpthread -lrt

clean:
	rm -rf *.o fifo* mq_* sh_*  dataserver client
