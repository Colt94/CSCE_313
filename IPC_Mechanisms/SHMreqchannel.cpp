#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "SHMreqchannel.h"
#include "reqchannel.h"


SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side)
:RequestChannel(_name, _side)
{
    string name1 = "sh_" + _name + "1";
    string name2 = "sh_" + _name + "2";
    
    bb1 = new SHMBoundedBuffer(name1);
    bb2 = new SHMBoundedBuffer(name2);
}

SHMRequestChannel::~SHMRequestChannel()
{
    delete bb1;
    delete bb2;
}

void SHMRequestChannel::cwrite(string msg)
{
    if(my_side == SERVER_SIDE)
        bb1->push(msg);
    else
        bb2->push(msg);
}

string SHMRequestChannel::cread()
{
    if(my_side == SERVER_SIDE)
        return bb2->pop();
    else  
        return bb1->pop();
    
}
