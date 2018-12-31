#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "KernelSemaphore.h"
using namespace std;


KernelSemaphore::KernelSemaphore(int _val, key_t key)
{
    semid = semget(key,1,IPC_CREAT | IPC_EXCL | 0666);
    if(semid == -1)
        semid = semget(key,1,0666);
    else
    {
        struct sembuf sb = {0,_val,0};
        if(semop(semid, &sb, 1) == -1)
            perror("semop");
    }
}

KernelSemaphore::~KernelSemaphore()
{
    char * n = const_cast<char*>(file.c_str());
	remove(n);
    semctl(semid, 0, IPC_RMID);
}

void KernelSemaphore::P()
{
    struct sembuf sb = {0,-1,0};
    if(semop(semid, &sb, 1) == -1)
        perror("semop");
}

void KernelSemaphore::V()
{
    struct sembuf sb = {0,1,0};
    if(semop(semid, &sb, 1) == -1)
        perror("semop");
}