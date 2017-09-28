//
//  UDP_Protocol.h
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#ifndef UDP_Protocol_h
#define UDP_Protocol_h
#include <chrono>
#include <string>
using namespace std;
#define NUM_SUC 2
#define NUM_PRE 2

class UDP_Server{
private:
//    int port;
//    string host_name;
//    int socket_fd;
//    bool heartbeat_flag;
//    int my_id;

	   
public:
//    UDP_Server();
//    UDP_Server(string host_, int port_);
//    ~UDP_Server();

    int send_msg(string dest_host_name, int dest_port, char* msg, int msg_length);
//    void send_Status_Msg(string dest_host_name, int dest_port, char* msg, int msg_length, int monitors[NUM_MONITOR]);
//    void send_HeartBeat_Msg(string dest_host_name, int dest_port, char* msg, int msg_length, int monitors[NUM_MONITOR]);

};


#endif /* UDP_Protocol_h */
