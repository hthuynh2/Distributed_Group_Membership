//
//  main.cpp
//  
//
//  Created by Hieu Huynh on 9/27/17.
//

#include <stdio.h>
#include "common.h"
#include <chrono>
#include <thread>


VM_info membership_list[NUM_VMS];
int successors[NUM_SUC];
int predecessors[NUM_PRE];
FILE* log_fp;
int my_id;
string my_id_str;
int my_socket_fd;
std::mutex mem_list_lock;
std::mutex successors_lock;
std::mutex predecessors_lock;
std::mutex log_fp_lock;


UDP_Client* my_listener;

string time_stamp;
string vm_hosts[NUM_VMS] = {
    "fa17-cs425-g13-01.cs.illinois.edu",
    "fa17-cs425-g13-02.cs.illinois.edu",
    "fa17-cs425-g13-03.cs.illinois.edu",
    "fa17-cs425-g13-04.cs.illinois.edu",
    "fa17-cs425-g13-05.cs.illinois.edu",
    "fa17-cs425-g13-06.cs.illinois.edu",
    "fa17-cs425-g13-07.cs.illinois.edu",
    "fa17-cs425-g13-08.cs.illinois.edu",
    "fa17-cs425-g13-09.cs.illinois.edu",
    "fa17-cs425-g13-10.cs.illinois.edu"
};

void heartbeat_checker_handler();
void get_membership_list();
void init_machine();
void msg_handler_thread(sting msg);
void heartbeat_sender_handler();
void heartbeat_checker_handler();



void get_membership_list(){
    Message my_msg;
    string request_msg = my_msg.create_J_msg();
    bool alive_vms[NUM_VMS] = {false};
    
    while(1){
        cout << "Requesting membership list from VM0...\n";
        temp_server.send_msg(vm_hosts[0], PORT, request_msg, request_msg.size());
        string r_msg = my_listener.read_msg_non_block(200);
        if(r_msg.size() == 0){
            continue;
        }
        if((r_msg[0] == 'R') && ((r_msg.size() -2 )%12 ==0)){
            //Can also call my_msg.handler_R_msg();
            int i = 0;
            while(r_msg[i] != '\n' && i < r_msg.size() && r_msg.size() >2){
                i++;
                int vm_num = r_msg[i] = '0';
                i++;    // skip '.'
                string vm_st[10];
                for(int j = 0; j < 10; j++){
                    vm_st[j] = r_msg[i+j];
                }
                i += 10;
                VM_info new_vm = {vm_num, vm_st, ALIVE, 0};
                membership_list[vm_num] = new_vm;
                alive_vms[vm_num] = true;
            }
            for(int i = 0 ; i < NUM_VMS; i++){
                if(i != my_id && alive_vms[i] == false){
                    VM_info new_vm = {i, "0", DEAD, 0};
                    membership_list[vm_num] = new_vm;
                }
                else if(i == my_id){
                    VM_info new_vm = {i, "0", ALIVE, 0};
                    membership_list[vm_num] = new_vm;
                }
            }
            break;
        }
    }
}

void init_machine(){
    //Init my_id and my_id_str
    char my_addr[512];
    gethostname(my_addr,512);
    for(int i = 0 ; i < NUM_VMS; i++){
        if(strncmp(my_addr, vm_hosts[i].c_str(), vm_hosts[i].size()) == 0){
            my_id = i;
            break;
        }
    }
    my_id_str = to_string(my_id);

    ///Initialize my_socket_fd
    string host_name = vm_host[my_id];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    string port_str = to_string(port);
    
    if ((rv = getaddrinfo(host_name.c_str();,port_str.c_str(), &hints, &servinfo)) != 0) {
        //        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        perror("getaddrinfo: failed \n");
        exit(1);
    }
    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((my_socket_fd = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
            perror("server: socket fail");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        break;
    }
    if(p == NULL){
        perror("server: socket fail to bind");
        exit(1);
    }
    freeaddrinfo(servinfo);
    ////////
    
    //Initialize UDP listener
    my_listener = new UDP_Client();
    
    //Get time_stamp
    time_t seconds;
    seconds = time (NULL);
    time_stamp = to_string(seconds);
    
    //Get membership_list
    if(my_id != 0){
        get_membership_list();
    }
    else{
        //Initialize all vms to dead. NEED TO CHANGE LATTER!!
        for(int i = 1 ; i < NUM_VMS; i++){
            if(i != my_id && alive_vms[i] == false){
                VM_info new_vm = {i, "0", DEAD, 0};
                membership_list[vm_num] = new_vm;
            }
        }
        VM_info new_vm = {1, "0", ALIVE, 0};
        membership_list[vm_num] = new_vm;
    }
    
    //Get Log file pointer
    string file_name("vm_");
    file_name.push_back((char)(my_id + '0'));
    log_fp = fopen(file_name, "w");
    
    //Init pre/successor
    for(int i = 0 ; i < NUM_SUC; i++){
        successors[i] = (my_id+i)%NUM_VMS;
        predecessors[i] = (my_id - i + NUM_VMS)% NUM_VMS;
    }
}

void msg_handler_thread(sting msg){
    Msg msg_handler();
    if(msg[0] == 'H'){
        if(msg.size() != H_MESSAGE_LENGTH)
            return;
        msg_handler.handle_H_msg(msg);
    }
    else if(msg[0] == 'N'){
        if(msg.size() != N_MESSAGE_LENGTH)
            return;
        msg_handler.handle_N_msg(msg);

    }
    else if(msg[0] == 'L'){
        if(msg.size() != L_MESSAGE_LENGTH)
            return;
        msg_handler.handle_L_msg(msg);
    }
    else if(msg[0] == 'J' && my_id == 0){
        if(msg.size() != J_MESSAGE_LENGTH)
            return;
        msg_handler.handle_J_msg(msg);
    }
//    else if(msg[0] == 'R'){       //Only receive this once when startup. Did it in init_machine
//        if((msg.size()-2)%12 != 0)
//            return;
//        msg_handler.handle_R_msg(msg);
//    }
}

void listener_thread_handler(){
    vector<std::thread> thread_vector;
    while(1){
        string msg = receive_msg();
        if(msg.size() == 0){
            continue;
        }
        std::thread th(msg_handler_thread,msg);
        thread_vector.push_back(std::move(thr1));
//        http://www.cplusplus.com/forum/unices/194352/
        thread_vector.back().join();
    }
}


void heartbeat_sender_handler(){
    string msg = create_H_msg();
    while(1){
        UDP_Server sender();
        successors_lock.lock();
        predecessors_lock.lock();
        for(int i = 0 ; i < NUM_SUC; i++){
            sender.send_msg(vm_hosts[successors[i]], PORT, msg, msg.size());
            sender.send_msg(vm_hosts[predecessors[i]], PORT, msg, msg.size());
        }
        predecessors_lock.unlock();
        successors_lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(HB_TIME));
    }
}

void heartbeat_checker_handler(){
    while(1){
        mem_list_lock.lock();
        successors_lock.lock();
        predecessors_lock.lock();
        time_t cur_time;
        cur_time = time (NULL);
        
        for(int i = 0 ; i < NUM_SUC; i ++){
            if(successors[i] >=0){
                if(membership_list[successors[i]].vm_status == ALIVE){
                    if(((cur_time - membership_list[successors[i]].vm_heartbeat) > HB_TIMEOUT) &&
                       (membership_list[successors[i]].vm_heartbeat != 0) ){
                        membership_list[successors[i]].vm_status = DEAD;
                        membership_list[successors[i]].vm_heartbeat = 0;
                        //WRITE TO FILE THAT THIS IS DEAD!!
                        //SEND MSG TO OTHER VMS
                    }
                    else{
                        membership_list[successors[i]].vm_heartbeat = cur_time;
                    }
                }
            }
            if(predecessors[i] >=0){
                if(membership_list[predecessors[i]].vm_status == ALIVE){
                    if(((cur_time - membership_list[predecessors[i]].vm_heartbeat) > HB_TIMEOUT) &&
                       (membership_list[predecessors[i]].vm_heartbeat != 0) ){
                        membership_list[predecessors[i]].vm_status = DEAD;
                        membership_list[predecessors[i]].vm_heartbeat = 0;
                        //WRITE TO FILE THAT THIS IS DEAD!!
                        //SEND MSG TO OTHER VMS
                    }
                    else{
                        membership_list[predecessors[i]].vm_heartbeat = cur_time;
                    }
                }
            }
        }
        predecessors_lock.unlock();
        successors_lock.unlock();
        mem_list_lock.unlock();
    }
}


int main(){
    init_machine();
    std::thread listener_thread(listener_thread_handler);
    std::thread heartbeat_sender_thread(heartbeat_sender_handler);
    std::thread heartbeat_checker_thread(heartbeat_checker_handler);
    listener_thread.join();
    heartbeat_sender_thread.join();
    heartbeat_checker_thread.join();
    
    return 0;
}








