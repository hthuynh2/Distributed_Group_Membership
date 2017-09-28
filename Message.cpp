//
//  Message.cpp
//  
//
//  Created by Hieu Huynh on 9/27/17.
//

#include <stdio.h>
#include "Message.h"

Message::Message(){
    sender.append(my_id_str);
    sender.append(".");
    sender.append(time_stamp);
}

string Message::create_J_msg(){
    string msg;
    msg.append("J");
    msg.append(sender);
    msg.append("\n");
    return msg;
}
string Message::create_R_msg(vector<string> vm_list){
    string msg;
    msg.append("R");
    for(int i = 0 ; i <(int) vm_list.size(); i++){
        msg.append(vm_list[i]);
    }
    msg.append("\n");
    return msg;
}

string Message::create_N_msg(){
    string msg = "";
    return msg;
}

string Message::create_L_msg(){
    string msg = "";
    return msg;
}

string Message::create_H_msg(){
    string msg("H");
    msg.append(sender);
    msg.append("\n");
    return msg;
}


void Message::handle_R_msg(string msg){
    int num_alive_vm = (msg.size() - 2)/12;
    
    mem_list_lock.lock();
    for(int i = 0 ; i < num_alive_vm; i++){
        int vm_num = msg[1+i*12] - '0';
        membership_list[vm_num].vm_status  = ALIVE;
        membership_list[vm_num].vm_id  = vm_num;
        membership_list[vm_num].vm_time_stamp  = msg.substr(3+i*12 ,10);
    }
    mem_list_lock.unlock();
    update_pre_successor();
    return;
}

void Message::handle_N_msg(string msg){
    cout << msg;
    return;
}

void Message::handle_L_msg(string msg){
    cout << msg;

    return;
}

void Message::handle_J_msg(string msg){
    int sender_id = msg[1] - '0';
    string sender_st =  msg.substr(3 ,10);
    vector<string> vm_list;
    
    mem_list_lock.lock();
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
    
    //Update membership_list
    mem_list_lock.unlock();
    string r_msg = create_R_msg(vm_list);
    UDP_Server my_sender;
    my_sender.send_msg(vm_hosts[sender_id], r_msg);
    
    //Update pre/sucessor
    update_pre_successor();
    //Send msg to other VMs
}

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
    else if(membership_list[sender_id].vm_status == DEAD){
        if(strcmp(membership_list[sender_id].vm_time_stamp.c_str(), sender_st.c_str()) != 0){  //New node has join //DO I need to check which timestamp is newer?
            //Should I add it to membership? YES. Add it and update the successor && precessor
            membership_list[sender_id].vm_heartbeat = 0;
            membership_list[sender_id].vm_time_stamp = sender_st;
            membership_list[sender_id].vm_status = ALIVE;
            mem_list_lock.unlock();
            update_pre_successor();
            return;
        }
        else{
            mem_list_lock.unlock();
            return;
        }
    }
}

void Message::update_pre_successor(){
    mem_list_lock.lock();
    successors_lock.lock();
    predecessors_lock.lock();
    int temp_suc[NUM_SUC];
    int temp_pre[NUM_PRE];
    
    int count = 0;
    //update successor
    for(int i = 0 ; i < NUM_VMS && count < NUM_SUC; i++){
        int target = (i + my_id + 1)% NUM_VMS;
        if(target == my_id){
            for(int j = 0 ; j < NUM_SUC - count; j++){
//                temp_suc[j + count] = (my_id + j +1) % NUM_VMS;
                temp_suc[j + count] = -1;
            }
            break;
        }
        else if(membership_list[target].vm_status == ALIVE){
            temp_suc[count] = target;
            count++;
        }
    }
    count = 0;
    for(int i = 0 ; i < NUM_VMS && count < NUM_PRE; i++){
        int target = (my_id - 1 - i + NUM_VMS)% NUM_VMS;
        if(target == my_id){
            for(int j = 0 ; j < NUM_SUC - count; j++){
//                temp_suc[j + count] = (my_id - j - 1)% NUM_VMS;
                temp_pre[j + count] = -1;
            }
            break;
        }
        else if(membership_list[target].vm_status == ALIVE){
            temp_pre[count] = target;
            count++;
        }
    }
    std::sort(std::begin(temp_suc), std::end(temp_suc));
    std::sort(std::begin(successors), std::end(successors));

    //Find all successors that are still alive but not int new successors list
    int idx = 0;
    for(idx = 0; idx < NUM_SUC; idx++){
        if(successors[idx] > temp_suc[NUM_SUC -1])
            break;
    }
    for(int i = idx; i < NUM_SUC; i++){
        if(successors[i] >=0 && membership_list[successors[i]].vm_status == ALIVE)
            membership_list[successors[i]].vm_heartbeat = 0;
    }
    
    for(int i = 0 ; i < NUM_SUC; i++){
        successors[i] = temp_suc[i];
    }
    
    std::sort(std::begin(temp_pre), std::end(temp_pre));
    std::sort(std::begin(predecessors), std::end(predecessors));
    
    //Find all precessor that are still alive but not int new successors list
    for(idx = 0 ; idx < NUM_SUC; idx++){
        if(temp_pre[idx] >= 0 )
            break;
    }
    int min_non_neg = temp_pre[idx];
    for(idx = NUM_PRE-1; idx >= 0; idx --){
        if(predecessors[idx] < min_non_neg){
            break;
        }
    }
    for(int i = idx; i >=0; i--){
        if(predecessors[i] >= 0 && membership_list[predecessors[i]].vm_status == ALIVE)
            membership_list[predecessors[i]].vm_heartbeat = 0;
    }
    
    for(int i = 0 ; i < NUM_SUC; i++){
        predecessors[i] = temp_pre[i];
    }
//    for(int i = 0 ; i < NUM_SUC; i++){
//        cout << successors[i] <<" ";
//    }
//    cout <<"\n";
    
    predecessors_lock.unlock();
    successors_lock.unlock();
    mem_list_lock.unlock();
}



//    char msg[MAX_BUF_LEN];
//    //create heartbeat msg
//    time_t seconds;
//    seconds = time (NULL);
//    string st = to_string(seconds);
//    //    cout << st <<"\n";
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





