#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
using namespace std;

class BoundedBuffer {
private:
	queue<string> q;	
    pthread_mutex_t mtx;
    pthread_cond_t upperBound;
    pthread_cond_t lowerBound;
    int cap;
public:
    BoundedBuffer(int);
	~BoundedBuffer();
	int size();
    void push (string);
    string pop();
};

#endif /* BoundedBuffer_ */
