
#ifndef _KernelSemaphore_H_                   
#define _KernelSemaphore_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sys/ipc.h>
#include <sys/sem.h>
using namespace std;


class KernelSemaphore
{
    private:
        int semid;
        key_t key;
        string file;

        /*
        struct sembuf
        {
            ushort sem_num;
            short sem_op;
            short sem_flg;
        };
        */
    public:
        KernelSemaphore(int _val, key_t key);
        ~KernelSemaphore();

        void P();
        void V();
};

#endif