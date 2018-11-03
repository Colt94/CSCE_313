#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std;


int main()
{/*
    char buf[] = "ab";
    int r = open("file.txt", O_RDONLY);
    int r1, r2, pid;
    r1 = dup(r);
    read(r,buf,1);
    cout << buf << endl;
    if(pid = fork() == 0){
    r1 = open("file.txt", O_RDONLY);
    }
    else
        waitpid(pid,NULL,0);

    read(r1,buf,1);
    cout << buf;
*/

char buf[] = "abc";
int r = open("file.txt", O_RDWR);
int r1 = 0;
int r2 = open("file.txt", O_RDWR);
dup2(r,r1);
read(r1,buf,1);
cout << buf <<endl;
read(r,buf,1);
cout << buf << endl;
read(r1,buf,1);
cout << buf << endl;
write(r,buf,1);
read(r,buf,1);
cout << buf << endl;
    return 0;
}