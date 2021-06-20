//
// Created by swagger on 2021/6/20.
//

#include "../include/Rtsp.h"
#include <unistd.h>
#include <cstring>
#include <iostream>


using std::cerr;
using std::cout;
using std::endl;
using std::string;

Rtsp::Rtsp(int fd)
:   fd_(fd)
{

    buf_ = new char[buf_size];

}

Rtsp::~Rtsp() {

    delete buf_;

}


void Rtsp::accept() {
    int len = ::read(fd_, buf_, buf_size);
    if ( len == 0 ){
        // 关闭操作
        return;
    }
    char* request_line = buf_;
    parser_request_header(request_line);
    PrintRequest();
}

void Rtsp::parser_request_header(char *data) {
    char* tmp;
    char* option;
    char* url;
    char* version;
    if ( (tmp = find_space(data)) != nullptr ){
        *tmp = '\0';
        tmp ++;
        option = data;
        data = tmp;
    }else { cerr << "option Error!" << endl;}
    if ( (tmp = find_space(tmp)) != nullptr ){
        *tmp = '\0';
        tmp ++;
        url = data;
        data = tmp;
    }else { cerr << "url Error!" << endl;}
    if ( (tmp = find_space(tmp)) != nullptr ){
        *tmp = '\0';
        version = data;
    }else { cerr << "version Error!" << endl;}

    option_ = string(option);
    version_ = string(version);
    request_ = string(url);

}

char* Rtsp::find_space(char *data) {
    if (!data)
        return nullptr;
    int len = strlen(data);
    for (int i=0; i<len; i++){
        if ( data[i] == ' ' || data[i] == '\t' )
            return data + i;
    }
    return nullptr;
}


void Rtsp::PrintRequest() {

    cout << "操作: " << option_ << endl;
    cout << "地址: " << request_ << endl;
    cout << "版本: " << version_ << endl;

}


//bool Rtsp::DecodeHeader(int &read_index, int &len, int &start_line) {
//
//
//    for (int i=read_index; i<len; i++){
//        if ( buf_[i] == '\r' ){
//
//        }
//    }
//
//}

