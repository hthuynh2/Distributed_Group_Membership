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
#include "Logger.h"
#include "Gossiper.h"
class Message{
private:
    string sender;
    
public:
    Message();
    string create_L_msg(int id_, string ts);
    string create_N_msg(int id_, string ts);
    string create_J_msg();
    string create_H_msg();
    string create_R_msg(vector<string> vm_list);
//    string create_G_msg(char type);

    
    void handle_R_msg(string msg);
    void handle_N_msg(string msg);

    void handle_L_msg(string msg);
    void handle_J_msg(string msg);
    void handle_H_msg(string msg);
    void update_pre_successor(bool haveLock);

};

#endif /* Message_h */
