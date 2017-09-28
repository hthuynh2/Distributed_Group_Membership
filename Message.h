//
//  Message.h
//  
//
//  Created by Hieu Huynh on 9/27/17.
//

#ifndef Message_h
#define Message_h
#define MAX_BUF_SIZE 1024
#include "common.h"
#include "UDP_Server.h"
#include "UDP_Client.h"
class Message{
private:
    string sender;
    
public:
    Message();
    string create_L_msg();
    string create_N_msg();
    string create_J_msg();
    string create_H_msg();
    string create_R_msg(vector<string> vm_list);

    
    void handle_R_msg(string msg);
    void handle_N_msg(string msg);

    void handle_L_msg(string msg);
    void handle_J_msg(string msg);
    void handle_H_msg(string msg);
    void update_pre_successor();

};

#endif /* Message_h */
