#ifndef _MQreqchannel_H_                   
#define _MQreqchannel_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sys/msg.h>
#include <sys/ipc.h>
using namespace std;

#include "reqchannel.h"
void EXITONERROR1 (string msg);



class MQRequestChannel : public RequestChannel {

	private:
		struct my_msgbuf {
		long mtype = 1;
		char mtext[256];
	};
		int send_id;
		int rec_id;
		string name1, name2;
		int file1, file2;
		key_t key1;
		key_t key2;
		string MQ_name(Mode _mode);
		char * create_file(string name);


	public:
		MQRequestChannel(const string _name, const Side _side);
		~MQRequestChannel();
		string cread();
		void cwrite(string msg);

		string name();
		/* Returns the name of the request channel. */

};

#endif