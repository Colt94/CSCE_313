#include "BoundedBuffer.h"
#include <string>
#include <queue>
using namespace std;

BoundedBuffer::BoundedBuffer(int _cap) {
	pthread_mutex_init(&mtx,NULL);
	pthread_cond_init(&upperBound,NULL);
	pthread_cond_init(&lowerBound,NULL);
	cap = _cap;
}

BoundedBuffer::~BoundedBuffer() {
	pthread_mutex_destroy(&mtx);
}

int BoundedBuffer::size() {
	pthread_mutex_lock(&mtx);
    return q.size();
	pthread_mutex_unlock(&mtx);
}

void BoundedBuffer::push(string str) {
	/*
	Is this function thread-safe??? Does this automatically wait for the pop() to make room 
	when the buffer if full to capacity???
	*/
	pthread_mutex_lock(&mtx);
	while(q.size() == cap)
		pthread_cond_wait(&upperBound,&mtx);

	q.push (str);
	pthread_cond_signal(&lowerBound);
	pthread_mutex_unlock(&mtx);
}

string BoundedBuffer::pop() {
	/*
	Is this function thread-safe??? Does this automatically wait for the push() to make data available???
	*/
	pthread_mutex_lock(&mtx);
	while(q.size() == 0)
		pthread_cond_wait(&lowerBound,&mtx);

	string s = q.front();
	q.pop();
	pthread_cond_signal(&upperBound);
	pthread_mutex_unlock(&mtx);
	return s;
}
