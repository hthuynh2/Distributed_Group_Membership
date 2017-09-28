//
//  UDP_Client.cpp
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#include <stdio.h>
#include "UDP_Client.h"

//UDP_Client::UDP_Client(){
//    
//}

/* This function read one line from the socketfd
 *Argument:     fd: fd to read from
 *              line: pointer to buffer to store data
 *Return:       Number of byte read
 */
int UDP_Client::getline_(int fd, char* line){
    char c = '\0';
    char buf[1024];
    int buf_idx = 0;
//    int flag = 0;
    while(recv(fd,&c,1,0) != 0){
        buf[buf_idx] = c;
        buf_idx++;
        if(c == '\n'){
            break;
        }
    }
    
    
//    while(read(fd,&c,1) != 0){
//        buf[buf_idx] = c;
//        buf_idx++;
//        if(c == '\r'){
//            flag = 1;
//        }
//        else if(c == '\n'){
//            if(flag == 1){
//                break;
//            }
//            else{
//                flag = 0;
//            }
//        }
//    }
    memcpy(line,buf,buf_idx);
    return buf_idx;
}

//Read only 1 msg
string UDP_Client::read_msg_non_block(int time_out){
    fd_set r_master, r_fds;            //set of fds
    FD_ZERO(&r_master);
    FD_ZERO(&r_fds);
    
    FD_SET(my_socket_fd, &r_master);
    timepnt begin;
    begin = clk::now();
    
    while(1){
        r_fds = r_master;
        if(select(my_socket_fd+1, &r_fds, NULL, NULL, NULL) == -1){
            perror("client: select");
            exit(4);
        }
        for(int i = 1 ; i <= my_socket_fd; i++){
            if(FD_ISSET(i, &r_fds)){
                char buf[MAX_BUF_SIZE];
                int buf_idx = getline_(i,buf);
                std::string myString(buf, buf_idx);
                return myString;
            }
        }
        if(std::chrono::duration_cast<unit_milliseconds>(clk::now() - begin).count() >= time_out)
            break;
    }
    string result = "";
    return result;
}


string UDP_Client::receive_msg(){
    char buf[1024];
    int numbytes = getline_(my_socket_fd, buf);
    std::string myString(buf, numbytes);
    return myString;
}


















