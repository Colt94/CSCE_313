
#ifndef _SHMRequestChannel_H_                   
#define _SHMRequestChannel_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sys/ipc.h>
#include "KernelSemaphore.h"
#include "SHMBoundedBuffer.h"
#include "reqchannel.h"
using namespace std;


class SHMRequestChannel : public RequestChannel {

    private:
        SHMBoundedBuffer *bb1, *bb2;
    public:
        void cwrite(string msg);
        string cread();
        SHMRequestChannel(const string _name, const Side _side);
        ~SHMRequestChannel();

};

#endif