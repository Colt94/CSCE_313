#ifndef _FIFOreqchannel_H_                   
#define _FIFOreqchannel_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
using namespace std;

#include "reqchannel.h"
void EXITONERROR0 (string msg);



class FIFORequestChannel : public RequestChannel {

    
	private:
	
		int wfd;
		int rfd;

		string pipe_name(Mode _mode);
		void create_pipe (string _pipe_name);
		void open_read_pipe(string _pipe_name);
		void open_write_pipe(string _pipe_name);

	public:
		FIFORequestChannel(const string _name, const Side _side);
		~FIFORequestChannel();
		string cread();
		void cwrite(string msg);
		string name();
		/* Returns the name of the request channel. */

		int read_fd();
		/* Returns the file descriptor used to read from the channel. */

		int write_fd();
		/* Returns the file descriptor used to write to the channel. */

};

#endif