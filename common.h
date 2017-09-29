//
//  common.h
//  
//
//  Created by Hieu Huynh on 9/27/17.
//

#ifndef common_h
#define common_h

#define PORT "4390"
#define J_MESSAGE_LENGTH 14
#define H_MESSAGE_LENGTH 14
#define N_MESSAGE_LENGTH 26
#define L_MESSAGE_LENGTH 26
#define NUM_SUC 2
#define NUM_PRE 2

#define VM_AND_TIMESTAMP_SIZE 12
#define ALIVE 1
#define DEAD 0
#define HB_TIME 500     //ms
#define HB_TIMEOUT 2
#define NUM_VMS 10

#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "limits.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>
#include <queue>
#include "Logger.h"


using namespace std;
typedef struct VM_info{
    int vm_id;
    string vm_time_stamp;
    int vm_status;
    long vm_heartbeat;
}VM_info;


//Need locks
extern VM_info membership_list[NUM_VMS];
extern int successors[NUM_SUC];
extern int predecessors[NUM_PRE];
extern std::mutex mem_list_lock;
extern std::mutex successors_lock;
extern std::mutex predecessors_lock;
extern std::mutex my_logger_lock;
extern Logger my_logger;


//No need lock
extern int my_id;
extern string my_id_str;
extern string time_stamp;
extern string vm_hosts[NUM_VMS];
extern int my_socket_fd;


#endif /* common_h */
