//
//  main.cpp
//  
//
//  Created by Hieu Huynh on 9/27/17.
//

#include "common.h"
#include "UDP_Client.h"
#include "UDP_Server.h"
#include "Message.h"
#include "Logger.h"
#include "Gossiper.h"


VM_info membership_list[NUM_VMS];
int successors[NUM_SUC];
int predecessors[NUM_PRE] ;
int my_id ;
string my_id_str ;
int my_socket_fd ;
std::mutex mem_list_lock;
std::mutex successors_lock;
std::mutex predecessors_lock;
std::mutex my_logger_lock;

Logger* my_logger;
UDP_Client* my_listener;
string time_stamp ;

string vm_hosts[NUM_VMS] =  {
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
void msg_handler_thread(string msg);
void heartbeat_sender_handler();
void heartbeat_checker_handler();


/*This function send request to VM0 to get membershiplist, and set the membership list based on response
 *Input:    None
 *Return:   None
 */
void get_membership_list(){
    Message my_msg;
    string request_msg = my_msg.create_J_msg();
    UDP_Server temp_server;
    
    for(int i = 0 ; i < NUM_VMS; i++){
        if(i != my_id)
            membership_list[i] = {i, "0", DEAD, 0};
        else
            membership_list[i] = {i, time_stamp, ALIVE, 0};
    }
    
    while(1){
        cout << "Requesting membership list from VM0...\n";
        temp_server.send_msg(vm_hosts[0], request_msg);
        string r_msg = my_listener->read_msg_non_block(200);
        if(r_msg.size() == 0){
            continue;
        }
        if((r_msg[0] == 'R') && ((r_msg.size() -2 )%12 ==0)){
            my_msg.handle_R_msg(r_msg);
            break;
        }
    }
}

/*This function initilize the vm. It sets my_id, my_id_str, my_logger, my_listener, membership list
 *Input:    None
 *Return:   None
 */
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
    
    my_logger = new Logger();
    
    //Init pre/successor
    for(int i = 0 ; i < NUM_SUC; i++){
        successors[i] = -1;
        predecessors[i] = -1;
    }
    
    ///Initialize my_socket_fd
    string host_name = vm_hosts[my_id];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(host_name.c_str(),PORT, &hints, &servinfo)) != 0) {
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
        bind(my_socket_fd, p->ai_addr, p->ai_addrlen);
//        if(bind(my_socket_fd, p->ai_addr, p->ai_addrlen) == -1){
//            close(my_socket_fd);
//            perror("listener: bind");
//            continue;
//        }
        break;
    }
    if(p == NULL){
        perror("server: socket fail to bind");
        exit(1);
    }
    freeaddrinfo(servinfo);
    
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
        //IF VM0, Initialize all vms to dead. NEED TO CHANGE LATTER!!
        for(int i = 1 ; i < NUM_VMS; i++){
            if(i != my_id){
                VM_info new_vm = {i, "0", DEAD, 0};
                membership_list[i] = new_vm;
            }
        }
        VM_info new_vm = {0, time_stamp, ALIVE, 0};
        membership_list[my_id] = new_vm;
    }
}

/* This function handles msg based on msg types
 *Input:    msg: message
 *Return:   None
 */
void msg_handler_thread(string msg){
    Message msg_handler;
    
    string str("Received: ");
    str.append(msg);
    
    my_logger_lock.lock();
    my_logger->write_to_file(str);
    my_logger_lock.unlock();
    
    if(msg[0] == 'G'){
        if(msg.size() != G_MESSAGE_LENGTH)
            return;
        Gossiper my_gossiper;
        string new_msg = my_gossiper.get_msg(msg, false);
        if(new_msg[0] == 'N'){
            if(new_msg.size() != N_MESSAGE_LENGTH)
                return;
            msg_handler.handle_N_msg(new_msg);
            
        }
        else if(new_msg[0] == 'L'){
            if(new_msg.size() != L_MESSAGE_LENGTH)
                return;
            msg_handler.handle_L_msg(new_msg);
        }
    }
    else if(msg[0] == 'H'){
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

/* This is thread handler to read and handle msg
 *Input:    None
 *Return:   None
 */
void listener_thread_handler(){
    vector<std::thread> thread_vector;
    while(1){
        string msg = my_listener->receive_msg();
        if(msg.size() == 0){
            continue;
        }
        std::thread th(msg_handler_thread,msg);
        thread_vector.push_back(std::move(th));
//        http://www.cplusplus.com/forum/unices/194352/
//        thread_vector.back().join();
    }
}

/* This is thread handler to send heartbeats to pre/successors
 *Input:    None
 *Return:   None
 */
void heartbeat_sender_handler(){
    Message my_msg;
    string msg = my_msg.create_H_msg();
    while(1){
        UDP_Server sender;
        successors_lock.lock();
        predecessors_lock.lock();
        for(int i = 0 ; i < NUM_SUC; i++){
            if(successors[i] >= 0 && successors[i] != my_id){
                string str("Send HB to VM ");
                str.append(to_string(successors[i]));
                str.append(": ");
                str.append(msg);
                
                my_logger_lock.lock();
                my_logger->write_to_file(str);
                my_logger_lock.unlock();
                
                sender.send_msg(vm_hosts[successors[i]], msg);
            }
            if(predecessors[i] >= 0 && predecessors[i] != my_id){
                string str("Send HB to VM ");
                str.append(to_string(successors[i]));
                str.append(": ");
                str.append(msg);
                
                my_logger_lock.lock();
                my_logger->write_to_file(str);
                my_logger_lock.unlock();
                sender.send_msg(vm_hosts[predecessors[i]], msg);
            }
        }
        predecessors_lock.unlock();
        successors_lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(HB_TIME));
    }
}

/* This is thread handler to check heartbeats of pre/successors. If timeout, set that node to DEAD, and send msg
 *Input:    None
 *Return:   None
 */
void heartbeat_checker_handler(){
    Message local_msg;

    while(1){
        mem_list_lock.lock();
        successors_lock.lock();
        predecessors_lock.lock();
        time_t cur_time;
        cur_time = time (NULL);
        
        for(int i = 0 ; i < NUM_SUC; i ++){
            if(successors[i] >=0){
                if(membership_list[successors[i]].vm_status == ALIVE){
                    int temp;
                    if((((temp = cur_time - membership_list[successors[i]].vm_heartbeat)) > HB_TIMEOUT) &&
                       (membership_list[successors[i]].vm_heartbeat != 0) ){
                        membership_list[successors[i]].vm_status = DEAD;
                        membership_list[successors[i]].vm_heartbeat = 0;
                        
                        //WRITE TO FILE THAT THIS IS DEAD!!
                        string str("VM");
                        str.push_back((char)(successors[i] + '0'));
                        str.append("with time stamp ");
                        str.append(membership_list[successors[i]].vm_time_stamp);
                        str.append(" Leave: ");
                        str.append("Last HB: ");
                        str.append(to_string(membership_list[successors[i]].vm_heartbeat));
                        str.append(" : ");
                        str.append(to_string(temp));
                        str.push_back('\n');
                        
                        my_logger_lock.lock();
                        my_logger->write_to_file(str);
                        my_logger_lock.unlock();
                        int fail_vm_id = successors[i];
                        string fail_vm_ts =membership_list[successors[i]].vm_time_stamp;
                        
                        //Update successors
                        local_msg.update_pre_successor(true);
                        
                        //SEND MSG TO OTHER VMS
                        string l_msg = local_msg.create_L_msg(fail_vm_id, fail_vm_ts);
                        Gossiper my_gossiper;
                        my_gossiper.send_Gossip(l_msg, true);
                    }
                }
            }
            if(predecessors[i] >=0){
                if(membership_list[predecessors[i]].vm_status == ALIVE){
                    int temp;

                    if((((temp =cur_time - membership_list[predecessors[i]].vm_heartbeat)) > HB_TIMEOUT) &&
                       (membership_list[predecessors[i]].vm_heartbeat != 0) ){
                        membership_list[predecessors[i]].vm_status = DEAD;
                        membership_list[predecessors[i]].vm_heartbeat = 0;

                        //WRITE TO FILE THAT THIS IS DEAD!!
                        string str("VM");
                        str.push_back((char)(predecessors[i] + '0'));
                        str.append("with time stamp ");
                        str.append(membership_list[predecessors[i]].vm_time_stamp);
                        str.append(" Leave: ");
                        str.append("Last HB: ");
                        str.append(to_string(membership_list[predecessors[i]].vm_heartbeat));
                        str.append(" : ");
                        str.append(to_string(temp));
                        str.push_back('\n');
                        
                        my_logger_lock.lock();
                        my_logger->write_to_file(str);
                        my_logger_lock.unlock();
                        
                        int fail_vm_id = predecessors[i];
                        string fail_vm_ts =membership_list[predecessors[i]].vm_time_stamp;
                        
                        //Update Precessors
                        local_msg.update_pre_successor(true);
                        
                        //SEND MSG TO OTHER VMS
                        string l_msg = local_msg.create_L_msg(fail_vm_id, fail_vm_ts);
                        Gossiper my_gossiper;
                        my_gossiper.send_Gossip(l_msg, true);
                    }
                }
            }
        }
        predecessors_lock.unlock();
        successors_lock.unlock();
        mem_list_lock.unlock();
    }
}

/* This is thread handler wait for user input to close file before Ctrl+C
 *Input:    None
 *Return:   None
 */
void user_input_handler(){
    while(1){
        string input;
        cin >> input;
        if(strncmp(input.c_str(), "quit", 4) == 0){
            my_logger_lock.lock();
            my_logger->close_log_file();
            my_logger_lock.unlock();
        }
    }
}

int main(){
    init_machine();
    cout <<"Successfully Initialize\n";
    std::thread listener_thread(listener_thread_handler);
    std::thread heartbeat_sender_thread(heartbeat_sender_handler);
    std::thread heartbeat_checker_thread(heartbeat_checker_handler);
    std::thread user_input_thread(user_input_handler);

    listener_thread.join();
    heartbeat_sender_thread.join();
    heartbeat_checker_thread.join();
    user_input_thread.join();
    
    return 0;
}








