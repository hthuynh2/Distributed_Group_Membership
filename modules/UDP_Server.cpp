//
//  UDP_Protocol.cpp
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#include <stdio.h>
#include "UDP_Server.h"
#define MAX_BUF_LEN 1024
#define NUM_MONITOR 4
#define HEART_BEAT_MSG_SIZE 15

//UDP_Server::UDP_Server(){
//    struct addrinfo hints, *servinfo, *p;
//    int rv;
//    memset(&hints, 0, sizeof(hints));
//    hints.ai_family = AF_UNSPEC;
//    hints.ai_socktype = SOCK_DGRAM;
//
//    if ((rv = getaddrinfo(vm_hosts[my_id].c_str(),PORT, &hints, &servinfo)) != 0) {
//        //        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//        perror("getaddrinfo: failed \n");
//        exit(1);
//    }
//
//    // loop through all the results and make a socket
//    for(p = servinfo; p != NULL; p = p->ai_next) {
//        if ((socket_fd = socket(p->ai_family, p->ai_socktype,
//                                p->ai_protocol)) == -1) {
//            perror("server: socket fail");
//            continue;
//        }
//        break;
//    }
//    if(p == NULL){
//        perror("server: socket fail to bind");
//        exit(1);
//    }
//    freeaddrinfo(servinfo);
//}
//
//UDP_Server::~UDP_Server(){
//    close(socket_fd);
//}

//
//
//
//void UDP_Server::send_HeartBeat_Msg(string dest_host_name, int dest_port, char* msg, int msg_length){
//    char msg[MAX_BUF_LEN];
//    //create heartbeat msg
//    time_t seconds;
//    seconds = time (NULL);
//    string st = to_string(seconds);
////    cout << st <<"\n";
//    msg[0] = 'H'    ;
//    if(st.size() < 10){
//        int idx = 10 - st.size();
//        memset((char*) (msg+1), '0', idx);
//        memcpy((char*)(msg+ 1 + idx), st.c_str(), st.size());
//    }
//    else if (st.size() == 10){
//        memcpy((char*)(msg+1), st.c_str(), st.size());
//    }
//    else{
//        memcpy((char*)(msg+ 1), (char*)(st.c_str() + (st.size() - 10)) , 10);
//    }
//    msg[11] = '.';
//    if(heartbeat_flag == true)
//        msg[12] = '1';
//    else
//        msg[12] = '0';
//
//    heartbeat_flag = !heartbeat_flag;
//
//    msg[13] = '\r';
//    msg[14] = '\n';
//
//    for(int i = 0 ; i < NUM_SUC; i++){
//        send_msg(vm_hosts[successors[i]], PORT, msg, HEART_BEAT_MSG_SIZE);
//    }
//    for(int i = 0 ; i < NUM_PRE; i++){
//        send_msg(vm_hosts[predecessors[i]], PORT, msg, HEART_BEAT_MSG_SIZE);
//    }
//    return ;
//}
//
//void UDP_Server::send_Status_Msg(string dest_host_name, int dest_port, char* msg, int msg_length, int monitors[NUM_MONITOR]){
//    return;
//}

int UDP_Server::send_msg(string dest_host_name, string msg){
    struct addrinfo hints, *servinfo;
    int rv;
    int numbyte = 0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(dest_host_name.c_str(),PORT, &hints, &servinfo)) != 0) {
        //        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        perror("getaddrinfo: failed \n");
        exit(1);
    }
    int buf_idx = 0;
    int msg_length = (int) msg.size();
    while(msg_length > 0){
        if((numbyte = sendto(my_socket_fd, (char*)(msg.c_str()+buf_idx), msg_length-buf_idx, 0,
                             servinfo->ai_addr, sizeof(*servinfo->ai_addr))) == -1){
            perror("Message: send");
            return -1;
        }
        buf_idx += numbyte;
        msg_length -= numbyte;
    }
    
    return numbyte;
}












