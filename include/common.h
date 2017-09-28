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


#define VM_AND_TIMESTAMP_SIZE 12
#define ALIVE 1
#define DEAD 0
#define HB_TIME 500     //ms
#define HB_TIMEOUT 2000

//Need locks
extern VM_info membership_list[NUM_VMS];
extern int successors[NUM_SUC];
extern int predecessors[NUM_PRE];
extern FILE* log_fp;
extern std::mutex mem_list_lock;
extern std::mutex successors_lock;
extern std::mutex predecessors_lock;
extern std::mutex log_fp_lock;



//No need lock
extern int my_id;
extern string my_id_str;
extern string time_stamp;
extern string vm_host[NUM_VMS];
extern int my_socket_fd;

typedef struct VM_info{
    int vm_id;
    string vm_time_stamp;
    int vm_status;
    long vm_heartbeat;
}VM_info;


#endif /* common_h */
