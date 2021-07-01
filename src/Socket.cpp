//
// Created by Swagger on 2021/7/1.
//

#include "../include/Socket.h"
#include <cstring>
#include <iostream>

using std::cerr;
using std::endl;
using std::cout;


Socket::Socket(std::string addr, int port)
:   addr_(std::move(addr)),
    port_(port)
{
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    Address local{};
    memset(&local, 0, sizeof(local));
    local.sin_addr.s_addr = inet_addr(addr_.c_str());
    local.sin_port = htons(port_);
    local.sin_family = AF_INET;
    int ret = ::bind(listenfd_, (sockaddr*)&local, sizeof(local));
    if ( ret < 0 ){
        /// for debug
        cerr << "bind() Error!" << endl;
        exit(ret);
    }
    ret = ::listen(listenfd_, 5);
    if ( ret < 0 ){
        /// for debug
        cerr << "listen() Error!" << endl;
        exit(ret);
    }
    cout << "socket init OK! " << endl;

}


Socket::~Socket() {

}

int Socket::getfd() {
    return listenfd_;
}