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

string make_time(int time){
    string ret("");
    int hour = time/3600;
    int min = (time % 3600)/60;
    int sec = (time %60);
    ret.append(to_string(hour));
    ret.push_back(':');
    ret.append(to_string(min));
    ret.push_back(':');
    ret.append(to_string(sec));
    ret.append(" : ");
    return ret;
}

void Logger::write_to_file(string msg){
    if(fp == NULL)
        return;
    time_t cur;
    cur = time (NULL);
    int elapse = cur - begin;
    string st = make_time(elapse);
    st.append(msg);
    fputs(st.c_str(), fp);
    return;
}

void Logger::close_log_file(){
    if(fp != NULL){
        fclose(fp);
    }
    fp = NULL;
    cout << "File is closed\n";
}

