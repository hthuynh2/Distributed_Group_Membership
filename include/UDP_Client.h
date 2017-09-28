//
//  UDP_Client.h
//  
//
//  Created by Hieu Huynh on 9/25/17.
//
//

#ifndef UDP_Client_h
#define UDP_Client_h
#include<string>
#include <chrono>
typedef std::chrono::high_resolution_clock clk;
typedef std::chrono::time_point<clk> timepnt;
typedef std::chrono::milliseconds unit_milliseconds;
typedef std::chrono::microseconds unit_microseconds;
typedef std::chrono::nanoseconds unit_nanoseconds;
#define MAX_BUF_SIZE 1024

using namespace std;
class UDP_Client{
private:
    
public:
//    UDP_Client();
    int receive_msg(char* buf);
    string read_msg_non_block(int time_out);
    int getline_(int fd, char* line){


//    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,

};

#endif /* UDP_Client_h */
//using namespace std;
//class UDP_Server{
//private:
//    int port;
//    string host_name;
//    int socket_fd;
//    
//public:
//    UDP_Server(string host, int port);
//    int send_msg(string dest_host_name, int dest_port, char* msg, int msg_length);
//    ~UDP_Server();
//    
//};
