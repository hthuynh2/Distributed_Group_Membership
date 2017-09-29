//
//  Logger.cpp
//  
//
//  Created by Hieu Huynh on 9/28/17.
//

#include "Logger.h"
#include "common.h"

Logger::Logger(){
    string file_name("vm_");
    file_name.push_back((char)(my_id + '0'));
    fp = fopen(file_name.c_str(), "w");
    begin = time (NULL);
}

void Logger::write_to_file(string msg){
    if(fp == NULL)
        return;
    time_t cur;
    cur = time (NULL);
    long elapse = cur - begin;
    string st(to_string(elapse));
    st.append(msg);
    
    fputs(st.c_str(), fp);
    return;
}

void Logger::close_log_file(){
    if(fp != NULL){
        fclose(fp);
    }
    cout << "File is closed\n";
}

