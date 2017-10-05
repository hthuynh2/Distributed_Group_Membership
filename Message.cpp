//
//  Message.cpp
//  
//
//  Created by Hieu Huynh on 9/27/17.
//

#include <stdio.h>
#include "Message.h"
#include <string>
#include <unordered_set>
using namespace std;
/*This constructor initialize the string sender used to create msg later
 */
Message::Message(){
    sender.append(my_id_str);
    sender.append(".");
    sender.append(time_stamp);
}

/*This method create a J msg to request membership list from VM0
 *Input:    None
 *Return:   message
 */
string Message::create_J_msg(){
    string msg;
    msg.append("J");
    msg.append(sender);
    msg.append("\n");
    return msg;
}

/*This method create a R msg to Response to the J msg
 *Input:    vm_list: vector of alive vm
 *Return:   R msg, which is a list of alive vms
 */
string Message::create_R_msg(vector<string> vm_list){
    string msg;
    msg.append("R");
    for(int i = 0 ; i <(int) vm_list.size(); i++){
        msg.append(vm_list[i]);
    }
    msg.append("\n");
    return msg;
}

//string create_G_msg(char type, string vm_id_st){
//    if(type != 'N' && type != 'L'){
//        string ret_msg("");
//        return ret_msg;
//    }
//    string ret_msg = ("G");
//    if(type == 'N'){
//        string n_msg = create_N_msg(vm_id_st);
//        ret_msg.append()
//    }
//    else (type == 'L'){
//
//    }
//}


/* Need to implement
 *
 */
string Message::create_N_msg(int id, string ts){
    string msg("N");
    msg.push_back((char) (id+'0'));
    msg.push_back('.');
    msg.append(ts);
    msg.push_back('\n');
    return msg;
}

/* Need to implement
 *
 */
string Message::create_L_msg(int id, string ts){
    string msg("L");
    msg.push_back((char) (id+'0'));
    msg.push_back('.');
    msg.append(ts);
    msg.push_back('\n');
    return msg;
}

/*This method create a H msg which is Heart Beat msg
 *Input:    none
 *Return:   Return heartbeat msg
 */
string Message::create_H_msg(){
    string msg("H");
    msg.append(sender);
    msg.append("\n");
    return msg;
}

/*This method handles R message. It updates membership list and pre/successors based on input R msg
 *Input:    msg: R msg
 *Output:   none
 */
void Message::handle_R_msg(string msg){
    cout << msg;
    string memlist_str("Current Membership List: ");

    int num_alive_vm = (msg.size() - 2)/12;
    mem_list_lock.lock();
    for(int i = 0 ; i < num_alive_vm; i++){
        int vm_num = msg[1+i*12] - '0';
        membership_list[vm_num].vm_status  = ALIVE;
        membership_list[vm_num].vm_id  = vm_num;
        membership_list[vm_num].vm_time_stamp  = msg.substr(3+i*12 ,10);
        //Logging
        memlist_str.append(to_string(vm_num));
        memlist_str.append(" ");

    }
    
    memlist_str.append("\n");

    my_logger_lock.lock();
    my_logger->write_to_file(memlist_str);
    my_logger_lock.unlock();
    
    successors_lock.lock();
    predecessors_lock.lock();
    update_pre_successor(true);
    
    predecessors_lock.unlock();
    successors_lock.unlock();
    mem_list_lock.unlock();
    return;
}

/*Need to Implement
 *
 *
 */
void Message::handle_N_msg(string msg){
    cout << msg;
    int sender_id = msg[1] - '0';
    string sender_st =  msg.substr(3 ,10);
    mem_list_lock.lock();
    //Update membership list
    membership_list[sender_id].vm_status  = ALIVE;
    membership_list[sender_id].vm_id  = sender_id;
    membership_list[sender_id].vm_time_stamp = sender_st;
    
    
    successors_lock.lock();
    predecessors_lock.lock();
    
    update_pre_successor(true);
    
    predecessors_lock.unlock();
    successors_lock.unlock();
    mem_list_lock.unlock();
    
    //Logging
    string memlist_str("Current Membership List: ");
    for(int i = 0 ; i < NUM_VMS; i++){
        if(membership_list[i].vm_status == ALIVE){
            memlist_str.append(to_string(i));
            memlist_str.push_back(' ');
        }
    }
    memlist_str.push_back('\n');
    my_logger_lock.lock();
    my_logger->write_to_file(memlist_str);
    my_logger_lock.unlock();
    
    return;
}

/*Need to Implement
 *
 *
 */
void Message::handle_L_msg(string msg){
    cout << msg;
    int sender_id = msg[1] - '0';
    string sender_st =  msg.substr(3 ,10);
    mem_list_lock.lock();
    
    //Update membership list
    membership_list[sender_id].vm_status  = DEAD;
    membership_list[sender_id].vm_id  = sender_id;
    membership_list[sender_id].vm_time_stamp = sender_st;
    
    
    successors_lock.lock();
    predecessors_lock.lock();
    
    update_pre_successor(true);
    
    predecessors_lock.unlock();
    successors_lock.unlock();
    mem_list_lock.unlock();
    
  
    //Logging
    string memlist_str("Current Membership List: ");
    for(int i = 0 ; i < NUM_VMS; i++){
        if(membership_list[i].vm_status == ALIVE){
            memlist_str.append(to_string(i));
            memlist_str.push_back(' ');
        }
    }
    memlist_str.push_back('\n');
    my_logger_lock.lock();
    my_logger->write_to_file(memlist_str);
    my_logger_lock.unlock();
    
    return;
}

/*This method handles J message. It creates a R msg and send back back to request VM
 *Input:    msg: J msg
 *Output:   none
 */
void Message::handle_J_msg(string msg){
    cout << msg;

    int sender_id = msg[1] - '0';
    string sender_st =  msg.substr(3 ,10);
    vector<string> vm_list;
    
    mem_list_lock.lock();

    //Spread msg to other VMs
    string n_msg = create_N_msg(sender_id, sender_st);
    Gossiper my_gossiper;
    my_gossiper.send_Gossip(n_msg, true);
    
    //Prepare response msg
    for(int i = 0 ; i < NUM_VMS; i++){
        if(membership_list[i].vm_status == ALIVE && i != sender_id){
            string vm_str;
            vm_str.push_back(membership_list[i].vm_id + '0');
            vm_str.push_back('.');
            vm_str.append(membership_list[i].vm_time_stamp);
            vm_list.push_back(vm_str);
        }
    }
    //Update membership list
    if(membership_list[sender_id].vm_status  != ALIVE || strcmp(membership_list[sender_id].vm_time_stamp.c_str(),sender_st.c_str()) != 0){
        membership_list[sender_id].vm_status  = ALIVE;
        membership_list[sender_id].vm_id  = sender_id;
        membership_list[sender_id].vm_time_stamp = sender_st;
    }
    
    string memlist_str("Current Membership List: ");
    for(int i = 0 ; i < NUM_VMS; i++){
        if(membership_list[i].vm_status == ALIVE){
            memlist_str.append(to_string(i));
            memlist_str.push_backg(' ');
        }
    }
    //Update membership_list
    mem_list_lock.unlock();

    memlist_str.append("\n");
    
    //////
    string log_msg("VM");
    log_msg.push_back((char)(sender_id+'0'));
    log_msg.append(" with time stamp ");
    log_msg.append(sender_st);
    log_msg.append(" joined.\n");
    
    
    my_logger_lock.lock();
    my_logger->write_to_file(log_msg);
    my_logger->write_to_file(memlist_str);

    my_logger_lock.unlock();
    
    //Update pre/sucessor
    update_pre_successor(false);

    //Send Response to requested VM
    string r_msg = create_R_msg(vm_list);
    UDP_Server my_sender;
    my_sender.send_msg(vm_hosts[sender_id], r_msg);
    
    //Logging
//    string memlist_str("Current Membership List: ");
//    for(int i = 0 ; i < NUM_VMS; i++){
//        if(membership_list[i].vm_status == ALIVE){
//            memlist_str.append(to_string(i));
//        }
//    }
//    my_logger_lock.lock();
//    my_logger->write_to_file(memlist_str);
//    my_logger_lock.unlock();
    
}

/*This method handles H message. It update the heartbeat of pre/successors
 *Input:    msg: H msg
 *Output:   none
 */
void Message::handle_H_msg(string msg){
    //Need to check if sender is in pre/suc list????
    int sender_id = msg[1] - '0';
    string sender_st =  msg.substr(3 ,10);
    time_t cur_time;
    mem_list_lock.lock();
    cur_time = time (NULL);
    
    if(membership_list[sender_id].vm_status == ALIVE && strcmp(sender_st.c_str(), membership_list[sender_id].vm_time_stamp.c_str()) == 0){
        membership_list[sender_id].vm_heartbeat = (long)cur_time;
        mem_list_lock.unlock();
        return;
    }
//    else if(membership_list[sender_id].vm_status == DEAD){
//        if(strcmp(membership_list[sender_id].vm_time_stamp.c_str(), sender_st.c_str()) != 0){  //New node has join //DO I need to check which timestamp is newer?
//            //Should I add it to membership? YES. Add it and update the successor && precessor
//            membership_list[sender_id].vm_heartbeat = 0;
//            membership_list[sender_id].vm_time_stamp = sender_st;
//            membership_list[sender_id].vm_status = ALIVE;
//            mem_list_lock.unlock();
//
//            string log_msg("VM");
//            log_msg.append(to_string(sender_id));
//            log_msg.append(" with time stamp ");
//            log_msg.append(sender_st);
//            log_msg.append(" joined.\n");
//            my_logger_lock.lock();
//            my_logger->write_to_file(log_msg);
//            my_logger_lock.unlock();
//            update_pre_successor(false);
//            return;
//        }
//    }
    mem_list_lock.unlock();
    return;
}

/*This method update the predecessors and successors of this VM based on alive vms
 *Input:    haveLock: indicate if already have mem_list_lock, successors_lock, predecessors_lock
 *Output:   None
 */
void Message::update_pre_successor(bool haveLock){
    if(haveLock == false){
        mem_list_lock.lock();
        successors_lock.lock();
        predecessors_lock.lock();
    }

    int temp_suc[NUM_SUC] = {-1};
    int temp_pre[NUM_PRE] = {-1};
    int count = 0;
    unordered_set<int> suc_set;
    
    //update successor
    int temp_idx = (my_id+1)%NUM_VMS;
    while(temp_idx != my_id && count < NUM_SUC){
        if(membership_list[temp_idx].vm_status == ALIVE){
            suc_set.insert(temp_idx);
            temp_suc[count] = temp_idx;
            count++;
        }
        temp_idx  = (temp_idx+1)%NUM_VMS;
    }
    
    //update precessors
    count  = 0;
    temp_idx = (my_id - 1 + NUM_VMS)%NUM_VMS;
    while(temp_idx != my_id && count < NUM_PRE){
        if(membership_list[temp_idx].vm_status == ALIVE && suc_set.find(temp_idx) == suc_set.end()){
            temp_pre[count] = temp_idx;
            count++;
        }
        temp_idx  = (temp_idx+1)%NUM_VMS;
    }

    //Set HB of old pre/successors that still ALIVE to 0
    for(int i = 0; i < NUM_SUC; i++){
        bool flag = false;
        for(int j = 0 ; j < NUM_SUC; j++){
            if(temp_suc[j] == successors[i]){
                flag = true;
            }
        }
        if(flag == true && membership_list[successors[i]].vm_status == ALIVE){
            string my_msg1("Set HB of VM ");
            my_msg1.append(to_string(successors[i]));
            my_msg1.append(" to 0");
            
            my_logger_lock.lock();
            my_logger->write_to_file(my_msg1);
            my_logger_lock.unlock();
            membership_list[successors[i]].vm_heartbeat = 0;
        }
    }
    
    for(int i = 0; i < NUM_SUC; i++){
        successors[i] = temp_suc[i];
    }
    
    for(int i = 0; i < NUM_PRE; i++){
        bool flag = false;
        for(int j = 0 ; j < NUM_PRE; j++){
            if(temp_pre[j] == predecessors[i]){
                flag = true;
            }
        }
        if(flag == true && membership_list[predecessors[i]].vm_status == ALIVE){
            string my_msg1("Set HB of VM ");
            my_msg1.append(to_string(predecessors[i]));
            my_msg1.append(" to 0");
            my_logger_lock.lock();
            my_logger->write_to_file(my_msg1);
            my_logger_lock.unlock();
            membership_list[predecessors[i]].vm_heartbeat = 0;
        }
    }
    
    for(int i = 0; i < NUM_SUC; i++){
        predecessors[i] = temp_pre[i];
    }
    
    string log_msg("Sucessors: ");
    for(int i = 0 ; i < NUM_SUC; i++){
        log_msg.append(to_string(successors[i]));
        log_msg.append(" ");
    }
    
    log_msg.append(" || Predecessors: ");
    for(int i = 0 ; i < NUM_SUC; i++){
        log_msg.append(to_string(predecessors[i]));
        log_msg.append(" ");
    }
    
    log_msg.push_back('\n');
    my_logger_lock.lock();
    my_logger->write_to_file(log_msg);
    my_logger_lock.unlock();
    
    //NEED TO DO: Need to send msg to old pre/successors that still alive
    if(haveLock == false){
        predecessors_lock.unlock();
        successors_lock.unlock();
        mem_list_lock.unlock();
    }
    
    //Logging
    string memlist_str("Current Membership List: ");
    for(int i = 0 ; i < NUM_VMS; i++){
        if(membership_list[i].vm_status == ALIVE){
            memlist_str.append(to_string(i));
            memlist_str.push_back(' ');
        }
    }
    memlist_str.push_back('\n');
    my_logger_lock.lock();
    my_logger->write_to_file(memlist_str);
    my_logger_lock.unlock();
    
}







