//
//  UDP_Client.cpp
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#include <stdio.h>
#include "UDP_Client.h"

/*This constructor sets the msg_buf_idx to 0
 */
UDP_Client::UDP_Client(){
    msg_buf_idx = 0;
}

/* This function parse buffer and return a vector of lines from buffer
 *Argument:     buf: buffer
 *              buf_size: buffer size
 *Return:       vector of lines from buffer
 */
vector<string> UDP_Client::buf_to_line(char* buf, int buf_size){
    vector<string> lines;
    string s(buf, buf_size);
    string delimiter = "\n";
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        token.push_back('\n');
        lines.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return lines;
}

/* This function read from fd and extract data into messages and store in msg queue
 *Argument:     fd: fd to read from
 *Return:       none
 */
void UDP_Client::getlines_(int fd){
    char buf[1024];
    int numbytes;
    if((numbytes = recv(fd, (char*)(msg_buf + msg_buf_idx), 1024 - msg_buf_idx, 0)) == -1){
        perror("UDP_Client: recv error\n");
        exit(1);
    }
    
    int idx ;
    for(idx = msg_buf_idx + numbytes-1; idx >=0 ; idx--){
        if(msg_buf[idx] == '\n')
            break;
    }
    if(idx == 0 && msg_buf[0] != '\n'){
        msg_buf_idx = msg_buf_idx + numbytes;
        return;
    }
    
    memcpy(buf, msg_buf, idx+1);
    memcpy(msg_buf, (char*)(msg_buf + idx+1), numbytes + msg_buf_idx - idx - 1);
    msg_buf_idx =  numbytes + msg_buf_idx - idx - 1;
    vector<string> lines = buf_to_line(buf, idx+1);
    for(int i = 0 ; i < (int)lines.size(); i++){
        msg_q.push(lines[i]);
    }
    return;
}

/*This function read 1 msg from socket and return after a specified time
 *Input:    time_out: wait time (in ms)
 *Return:   Message if exist. Return empty string if timeout and not received any msg.
*/
string UDP_Client::read_msg_non_block(int time_out){
    fd_set r_master, r_fds;
    FD_ZERO(&r_master);
    FD_ZERO(&r_fds);
    
    if(!msg_q.empty()){
        string ret = msg_q.front();
        msg_q.pop();
        return ret;
    }

    FD_SET(my_socket_fd, &r_master);
    timepnt begin;
    begin = clk::now();
    
    while(1){
        r_fds = r_master;
        struct timeval t_out;
        t_out.tv_sec = 0;
        t_out.tv_usec = time_out*1000;
        if(select(my_socket_fd+1, &r_fds, NULL, NULL, &t_out) == -1){
            perror("client: select");
            exit(4);
        }
        for(int i = 1 ; i <= my_socket_fd; i++){
            if(FD_ISSET(i, &r_fds)){
                getlines_(i);
                if(!msg_q.empty()){
                    string ret = msg_q.front();
                    msg_q.pop();
                    return ret;
                }
            }
        }
        break;
    }
    string result = "";
    return result;
}

/*This function read 1 msg from msg queue. If msg queue is empty, wait until receive msg
 *Input:    None
 *Return:   Message
 */
string UDP_Client::receive_msg(){
    //If there is msg in msg_q, return the oldest msg
    if(!msg_q.empty()){
        string ret_msg = msg_q.front();
        msg_q.pop();
        return ret_msg;
    }
    while(1){
        if(!msg_q.empty())
            break;
        getlines_(my_socket_fd);
    }
    string ret_msg = msg_q.front();
    msg_q.pop();
    return ret_msg;
}


















