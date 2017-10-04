//
//  Gossiper.cpp
//  
//
//  Created by Hieu Huynh on 10/3/17.
//

#include "Gossiper.h"
#include <random>
#include <set>

Gossiper::Gossiper(){
    init_TTL = 2;
}

void send_gossip_helper(vector<string> msg){
    vector<int> alive_id_array;

    mem_list_lock.lock();
    for(int i = 0 ; i < NUM_VMS; i++){
        if(membership_list[i].vm_status == ALIVE && membership_list[i].vm_id != my_id){
            alive_id_array.push_back(membership_list[i].vm_id);
        }
    }
    mem_list_lock.unlock();
    if(alive_id_array.size() == 0){
        return;
    }
    
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,alive_id_array.size()-1);
    
    UDP_Server local_sender;
    for(int i = 0; i < (int)msg.size(); i++){
        set<int> receivers;
        while (receivers.size() != 3 && receivers.size() < alive_id_array.size()) {
            int temp = distribution(generator);
            if(receivers.find(temp) == receivers.end()){
                receivers.insert(temp);
            }
        }
        for(auto it = receivers.begin(); it != receivers.end(); it++){
            cout << "Send: "<<msg[i] << " to " << vm_hosts[alive_id_array[*it]] <<"\n";
            local_sender.send_msg(vm_hosts[alive_id_array[*it]], msg[i]);
        }
    }
}

void Gossiper::send_Gossip(string msg){
    cout << "Gossip: " << msg;
    string g_msg("G");
    vector<string> msg_vector;
    g_msg.append(msg);
    g_msg.push_back('\n');
    
    int cur_ttl = init_TTL;
    while(cur_ttl >=0){
        g_msg[g_msg.size() - 2] = (char) (cur_ttl + '0');
        msg_vector.push_back(g_msg);
        cur_ttl--;
    }
    
    msg_vector.push_back(g_msg);
    send_gossip_helper(msg_vector);
}

string Gossiper::get_msg(string gossip_msg){
    int cur_ttl = gossip_msg[gossip_msg.size() - 2];
    
    string my_msg;
    my_msg = gossip_msg.substr(1,14);
    my_msg[my_msg.size() - 1] = '\n';
    if(cur_ttl == 0){
        return my_msg;
    }
    
    int vm_id = gossip_msg[2];
    char msg_type = gossip_msg[1];
    bool flag = false;
    
    mem_list_lock.lock();
    if((membership_list[vm_id].vm_status == ALIVE && msg_type == 'L') ||(membership_list[vm_id].vm_status == DEAD && msg_type == 'J')){
        flag = true;
    }
    mem_list_lock.unlock();
    
    if(flag == false){
        return my_msg;
    }
    
    cur_ttl--;
    vector<string> msg_v;
    while(cur_ttl >= 0){
        gossip_msg[gossip_msg.size() -2] = (char)(cur_ttl + '0');
        msg_v.push_back(gossip_msg);
        cur_ttl--;
    }
    
    send_gossip_helper(msg_v);
    return my_msg;
}














