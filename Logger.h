//
//  Logger.hpp
//  
//
//  Created by Hieu Huynh on 9/28/17.
//

#ifndef Logger_h
#define Logger_h

#include<string>
using namespace std;
class Logger{
private:
    FILE* fp;
    time_t begin;

    
public:
    Logger();
    void write_to_file(string msg);
    void close_log_file();
};




#endif /* Logger_hpp */
