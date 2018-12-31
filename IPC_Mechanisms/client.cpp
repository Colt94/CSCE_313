/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <signal.h>
#include <csignal>

#include <sys/time.h>
#include <cassert>
#include <assert.h>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "reqchannel.h"
#include "FIFOreqchannel.h"
#include "MQreqchannel.h"
#include "BoundedBuffer.h"
#include "SHMreqchannel.h"
#include "Histogram.h"
using namespace std;
  
Histogram hist;

struct request_data
{
    int num_of_req;
    string name;
    BoundedBuffer *safebuff;

    request_data(int num, string n, BoundedBuffer *buff)
    {
        num_of_req = num;
        name = n;
        safebuff = buff;
    }
};

struct worker_data
{
    RequestChannel *channel;
    BoundedBuffer *buffer;
    BoundedBuffer *SQ1;
    BoundedBuffer *SQ2;
    BoundedBuffer *SQ3;
    Histogram *histo;
};


struct stats_data
{
    string name;
    int num_of_request;
    BoundedBuffer *SQ;
    Histogram *histo;
};


void* request_thread_function(void* arg) {
	/*
		Fill in this function.

		The loop body should require only a single line of code.
		The loop conditions should be somewhat intuitive.

		In both thread functions, the arg parameter
		will be used to pass parameters to the function.
		One of the parameters for the request thread
		function MUST be the name of the "patient" for whom
		the data requests are being pushed: you MAY NOT
		create 3 copies of this function, one for each "patient".
	 */

	request_data *data_struct = (request_data*)arg;

    for(int i = 0; i < data_struct->num_of_req; i++)
    {
        data_struct->safebuff->push(data_struct->name);
    }
}

void* worker_thread_function(void* arg) {
    /*
		Fill in this function. 

		Make sure it terminates only when, and not before,
		all the requests have been processed.

		Each thread must have its own dedicated
		RequestChannel. Make sure that if you
		construct a RequestChannel (or any object)
		using "new" that you "delete" it properly,
		and that you send a "quit" request for every
		RequestChannel you construct regardless of
		whether you used "new" for it.
     */

    worker_data *worker_struct = (worker_data*)arg;

    while(true) {
        string request = worker_struct->buffer->pop();
		worker_struct->channel->cwrite(request);

		if(request == "quit") {
			//delete worker_struct->channel;
            break;
        }else{
			string response = worker_struct->channel->cread();
            if(request == "data John Smith")
                worker_struct->SQ1->push(response);
            else if(request == "data Jane Smith")
                worker_struct->SQ2->push(response);
            else
                worker_struct->SQ3->push(response);
		}
    }
}

void* stat_thread_function(void* arg) {
    /*
		Fill in this function. 

		There should 1 such thread for each person. Each stat thread 
        must consume from the respective statistics buffer and update
        the histogram. Since a thread only works on its own part of 
        histogram, does the Histogram class need to be thread-safe????

     */

    stats_data *stat_struct = (stats_data*)arg;
    string patient = stat_struct->name;
    for(int i = 0; i < stat_struct->num_of_request; i++) {
        string response = stat_struct->SQ->pop();
        stat_struct->histo->update(patient,response);   
    }
}


void signal_handler(int sig)
{
    system("clear");
    hist.print();
    alarm(2);
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    //alarm(2);
    //signal(SIGALRM, signal_handler);
    
    
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int b = 1; // default capacity of the request buffer, you should change this default
    char * IPC;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:b:i:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
            case 'b':
                b = atoi (optarg);
                break;
            case 'i':
                IPC = optarg;
		}
    }

    int pid = fork();
	if (pid == 0){
        execl("dataserver", "dataserver", argv[1], argv[2], NULL);
		//execl("dataserver", (char*) NULL);
	}
	else {
        
        struct timeval tp_start, tp_end;
        gettimeofday(&tp_start,0);

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;
        cout << "b == " << b << endl;
        cout << "i == " << IPC << endl;
        RequestChannel *chan;
        if((strcmp(IPC, "f") == 0))
        {
            cout << "FIFO selected" << endl;
            chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        else if((strcmp(IPC, "q") == 0))
        { 
            cout << "MQ selected" << endl;
            chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE);
        }  
        
        else if((strcmp(IPC, "s") == 0))
        {
            cout << "SHM selected" << endl;
            chan = new SHMRequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        
        BoundedBuffer request_buffer(b);
		
//---------------------------REQUEST THREADS-------------------------------------------------
        request_data data1(n,"data John Smith",&request_buffer);
        request_data data2(n,"data Jane Smith",&request_buffer);
        request_data data3(n,"data Joe Smith" ,&request_buffer);
        pthread_t rt1,rt2,rt3;
        pthread_create(&rt1,NULL,request_thread_function, &data1);
        pthread_create(&rt2,NULL,request_thread_function, &data2);
        pthread_create(&rt3,NULL,request_thread_function, &data3);
//---------------------------WORKER THREADS---------------------------------------------------
        if((b/3) == 0)
            b = 3;
        
        BoundedBuffer statqueue1(b/3);
        BoundedBuffer statqueue2(b/3);
        BoundedBuffer statqueue3(b/3);

        pthread_t wtid[w];
        worker_data workers[w];
        RequestChannel * workerChannels[w];

        
        if((strcmp(IPC, "f") == 0))
        {
            for(int i = 0; i < w; i++)
            {
                chan->cwrite("newchannel");
                string s = chan->cread();
                workerChannels[i] = new FIFORequestChannel(s, RequestChannel::CLIENT_SIDE);
                workers[i].SQ1 = &statqueue1;
                workers[i].SQ2 = &statqueue2;
                workers[i].SQ3 = &statqueue3;
                workers[i].channel = workerChannels[i];
                workers[i].buffer = &request_buffer;
                workers[i].histo = &hist;
                pthread_create(&wtid[i],NULL, worker_thread_function,&workers[i]);
            }
        }
        else if((strcmp(IPC, "q") == 0))
        {
            for(int i = 0; i < w; i++)
            {
                chan->cwrite("newchannel");
                string s = chan->cread();
                workerChannels[i] = new MQRequestChannel(s, RequestChannel::CLIENT_SIDE);
                workers[i].SQ1 = &statqueue1;
                workers[i].SQ2 = &statqueue2;
                workers[i].SQ3 = &statqueue3;
                workers[i].channel = workerChannels[i];
                workers[i].buffer = &request_buffer;
                workers[i].histo = &hist;
                pthread_create(&wtid[i],NULL, worker_thread_function,&workers[i]);
            }
        }
        
        else if((strcmp(IPC, "s") == 0))
        {
            for(int i = 0; i < w; i++)
            {
                chan->cwrite("newchannel");
                string s = chan->cread();
                workerChannels[i] = new SHMRequestChannel(s, RequestChannel::CLIENT_SIDE);
                workers[i].SQ1 = &statqueue1;
                workers[i].SQ2 = &statqueue2;
                workers[i].SQ3 = &statqueue3;
                workers[i].channel = workerChannels[i];
                workers[i].buffer = &request_buffer;
                workers[i].histo = &hist;
                pthread_create(&wtid[i],NULL, worker_thread_function,&workers[i]);
            }
        }    
        
//----------------------------STATISTICS THREADS------------------------------------------------------------
        //Histogram hist;
        pthread_t stid[3];
        stats_data stats[3];

        stats[0].SQ = &statqueue1;
        stats[0].name = "data John Smith";
        stats[0].histo = &hist;
        stats[0].num_of_request = n;
        stats[1].SQ = &statqueue2;
        stats[1].name = "data Jane Smith";
        stats[1].histo = &hist;
        stats[1].num_of_request = n;
        stats[2].SQ = &statqueue3;
        stats[2].name = "data Joe Smith";
        stats[2].histo = &hist;
        stats[2].num_of_request = n;

        for(int i = 0 ; i < 3; i++)
            pthread_create(&stid[i],NULL,stat_thread_function, &stats[i]);

//----------------------------------------------------------------------------------------
        pthread_join(rt1,NULL);
        pthread_join(rt2,NULL);
        pthread_join(rt3,NULL);

        cout << "Done populating request buffer" << endl;

        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
    
        cout << "done." << endl;
        
        
        for(int i = 0; i < w; i++)
            pthread_join(wtid[i], NULL);

        for(int i = 0; i < w; i++)
            delete workerChannels[i];

        for(int i = 0 ; i < 3; i++)
            pthread_join(stid[i],NULL);

        chan->cwrite ("quit"); 
        delete chan;
        cout << "All Done!!!" << endl; 

        

        gettimeofday(&tp_end, 0);
        long sec = tp_end.tv_sec - tp_start.tv_sec;
        long musec = tp_end.tv_usec - tp_start.tv_usec;
        if (musec < 0) {
            musec += (int)1e6;
            sec--;
        } 
        cout << endl;
        cout << "Time taken:  sec = " << sec << "  musec = " << musec << endl << endl;

		hist.print ();
    }
}
