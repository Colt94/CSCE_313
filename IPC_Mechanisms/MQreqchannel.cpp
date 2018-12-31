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

#include "MQreqchannel.h"
#include "reqchannel.h"

int nchans = 0;

void EXITONERROR1 (string msg){
	perror (msg.c_str());
	exit (-1);
}

char * MQRequestChannel::create_file(string name)
{
	char * n = const_cast<char*>(name.c_str());
	open(n, O_CREAT, O_RDWR);
	return n;
}

std::string MQRequestChannel::MQ_name(Mode _mode) {
	std::string pname = "mq_" + my_name;

	if (my_side == CLIENT_SIDE) {
		if (_mode == READ_MODE)
			pname += "1";
		else
			pname += "2";
	}
	else {
	// SERVER_SIDE 
		if (_mode == READ_MODE)
			pname += "2";
		else
			pname += "1";
	}


	return pname;
}

MQRequestChannel::MQRequestChannel(const string _name, const Side _side)
:RequestChannel(_name, _side)
{
	
	
	if (_side == SERVER_SIDE)
	{
		name1 = MQ_name(WRITE_MODE).c_str();
		name2 = MQ_name(READ_MODE).c_str(); 
		char * n1 = const_cast<char*>(name1.c_str());
		char * n2 = const_cast<char*>(name2.c_str());
		file1 = creat(n1, 600);
		file2 = creat(n2, 600);
		key1 = ftok(n1, 100);
		key2 = ftok(n2, 100);

		send_id = msgget(key1, 0644 | IPC_CREAT);
		rec_id = msgget(key2, 0644 | IPC_CREAT);
		if(send_id < 0)
			perror ("message queue error");
		
		close(file1);
		close(file2);
		
	}
	else
	{
		name1 = MQ_name(READ_MODE).c_str();
		name2 = MQ_name(WRITE_MODE).c_str();
		char * n1 = const_cast<char*>(name1.c_str());
		char * n2 = const_cast<char*>(name2.c_str());
		file1 = creat(n1, 600);
		file2 = creat(n2, 600);
		key1 = ftok(n1, 100);
		key2 = ftok(n2, 100);
		rec_id = msgget(key1, 0644 | IPC_CREAT);
		send_id = msgget(key2, 0644 | IPC_CREAT);
		if(rec_id < 0)
			perror ("message queue error");
		
		close(file1);
		close(file2);
		
	}

	//nchans = nchans + 2;

}

MQRequestChannel::~MQRequestChannel()
{
	//cout << "Deleting: " << name1 << endl;
	//cout << "Deleting: " << name2 << endl;
	char * n1 = const_cast<char*>(name1.c_str());
	char * n2 = const_cast<char*>(name2.c_str());
	remove(n1);
	remove(n2);
	msgctl(send_id, IPC_RMID, NULL);
	msgctl(rec_id, IPC_RMID, NULL);
}

void MQRequestChannel::cwrite(string msg)
{
	struct my_msgbuf buf;
	buf.mtype = 1;
	char * m = const_cast<char*>(msg.c_str());
	strncpy(buf.mtext, m, 256);
	
	int len = strlen(buf.mtext);
	
	if (msgsnd(send_id, &buf, len+1, 0) == -1)
		perror ("msgsnd");
	
}

string MQRequestChannel::cread()
{
	struct my_msgbuf buf;
	buf.mtype = 1;
	
	if (msgrcv(rec_id, &buf, sizeof buf.mtext, 0, 0) <= 0) 
		perror("msgrcv");
	string s = buf.mtext;
	return s;
}

