//
//  Gossiper.h
//  
//
//  Created by Hieu Huynh on 10/3/17.
//

#ifndef Gossiper_h
#define Gossiper_h
#include "UDP_Server.h"
#include "UDP_Client.h"
#include "Message.h"
#include "common.h"

class Gossiper{
private:
    int init_TTL;
    
public:
    Gossiper();
    void send_Gossip(string msg, bool haveLock);
    string get_msg(string gossip_msg, bool haveLock);

};

#endif /* Gossiper_h */
