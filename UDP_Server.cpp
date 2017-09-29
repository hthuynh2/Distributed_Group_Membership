//
//  UDP_Protocol.cpp
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#include "UDP_Server.h"
#define MAX_BUF_LEN 1024
#define NUM_MONITOR 4
#define HEART_BEAT_MSG_SIZE 15

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












