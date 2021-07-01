//
// Created by Swagger on 2021/7/1.
//

#ifndef RTSP_SOCKET_H
#define RTSP_SOCKET_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

class Socket{
public:

    typedef sockaddr_in Address;

    Socket(std::string addr, int port);
    ~Socket();

    int getfd();

private:

    int listenfd_;              // 监听fd
    std::string addr_;          // 地址
    int port_;                  // 端口


};




#endif //RTSP_SOCKET_H
