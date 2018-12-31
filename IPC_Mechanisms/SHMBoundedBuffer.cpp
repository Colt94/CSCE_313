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
#include <sys/shm.h>
#include <sys/ipc.h>
#include "KernelSemaphore.h"
#include "SHMBoundedBuffer.h"
using namespace std;


SHMBoundedBuffer::SHMBoundedBuffer(string name)
{
    file = name;
    char * n = const_cast<char*>(name.c_str());
    int temp = creat(n, 600);
    kshm = ftok(n, 0);
    kfull = ftok(n,1);
    kempty = ftok(n,2);

    shmid = shmget(kshm, 100, 0644 | IPC_CREAT);
    if(shmid == -1)
        perror("shmget");
    buff = (char*)shmat(shmid,0,0);
    int * temp2 = (int *)buff;
    if(*temp2 == -1)
        perror("shmat");

    f = new KernelSemaphore(0, kfull);
    e = new KernelSemaphore(1, kempty);
    close(temp);
}

SHMBoundedBuffer::~SHMBoundedBuffer()
{
    shmctl(shmid, 0, IPC_RMID);
    remove(const_cast<char*>(file.c_str()));
    delete f;
    delete e;
}

void SHMBoundedBuffer::push(string msg)
{
    e->P();
    char * m = const_cast<char*>(msg.c_str());
    strncpy(buff, m, 100); 
    f->V();
}

string SHMBoundedBuffer::pop()
{
    f->P();
    string x = buff;
    e->V();
    return x;
}