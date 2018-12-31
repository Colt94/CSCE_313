
#ifndef _SHMBoundedBuffer_H_                   
#define _SHMBoundedBuffer_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <queue>
#include "KernelSemaphore.h"
using namespace std;

class SHMBoundedBuffer
{
    private:
        int shmid;
        queue<string> q;
        char * buff;
        string file;
        key_t kshm, kfull, kempty;
        KernelSemaphore * f;
        KernelSemaphore * e;

    public:
        SHMBoundedBuffer(string name);
        ~SHMBoundedBuffer();
        string pop();
        void push(string msg);

};

#endif