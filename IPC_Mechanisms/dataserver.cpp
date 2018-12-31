#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "reqchannel.h"
#include "FIFOreqchannel.h"
#include "MQreqchannel.h"
#include "SHMreqchannel.h"
#include <pthread.h>
#include <sys/msg.h>
#include <sys/ipc.h>
using namespace std;


int nchannels = 0;
pthread_mutex_t newchannel_lock;
void* handle_process_loop (void* _channel);
int control = 0;
char * ipc;


void process_newchannel(RequestChannel* _channel, char * type) {
	nchannels ++;
	string new_channel_name = "data" + to_string(nchannels) + "_";
	_channel->cwrite(new_channel_name);
	RequestChannel * data_channel;

	if((strcmp(type, "f") == 0))
	{
		data_channel = new FIFORequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
	}
	else if((strcmp(type, "q") == 0))
	{
		data_channel = new MQRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
	}
	else if((strcmp(type, "s") == 0))
	{
		data_channel = new SHMRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
	}
	
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, handle_process_loop, data_channel) < 0 ) {
		EXITONERROR ("");
	}
	
}

void process_request(RequestChannel* _channel, string _request, char * ipc) {

	if (_request.compare(0, 5, "hello") == 0) {
		_channel->cwrite("hello to you too");
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		_channel->cwrite(to_string(rand() % 100));	
	}
	else if (_request.compare(0, 10, "newchannel") == 0) {
		process_newchannel(_channel, ipc);
	}
	else {
		_channel->cwrite("unknown request");
	}
}

void* handle_process_loop (void* _channel) {
	RequestChannel* channel = (RequestChannel *) _channel;
	
	for(;;) {
		string request = channel->cread();
		if (request.compare("quit") == 0) {
			break;                  // break out of the loop;
		}
		process_request(channel, request, ipc);
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
	ipc = argv[2];
	newchannel_lock = PTHREAD_MUTEX_INITIALIZER;
	if((strcmp(ipc, "f") == 0))
	{
		FIFORequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
		handle_process_loop(&control_channel);
	}
	else if((strcmp(ipc, "q") == 0))
	{
		MQRequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
		handle_process_loop(&control_channel);
	}
	else if((strcmp(ipc, "s") == 0))
	{
		SHMRequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
		handle_process_loop(&control_channel);
	}
		
}

