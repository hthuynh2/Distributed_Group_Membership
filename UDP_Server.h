//
//  UDP_Protocol.h
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#ifndef UDP_Protocol_h
#define UDP_Protocol_h
#include "common.h"
using namespace std;
#define NUM_SUC 2
#define NUM_PRE 2

class UDP_Server{
private:
	   
public:
    int send_msg(string dest_host_name, string msg);

};


#endif /* UDP_Protocol_h */
