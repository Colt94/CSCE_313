/* 
    File: requestchannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

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

#include "reqchannel.h"

void EXITONERROR (string msg){
	perror (msg.c_str());
	exit (-1);
}


RequestChannel::RequestChannel(const std::string _name, const Side _side) :
my_name(_name), my_side(_side), side_name((_side == RequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT")
{}


RequestChannel::~RequestChannel()
{
	//cout << "Closing >> " << my_name << " >> " << my_side << endl;
}





