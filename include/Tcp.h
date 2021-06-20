//
// Created by swagger on 2021/6/20.
//

#ifndef RTSP_TCP_H
#define RTSP_TCP_H

#include <iostream>


struct sockaddr_in;
struct epoll_event;

class Rtsp;

class Tcp{
public:

    Tcp(std::string address, int port);

    ~Tcp();

    void run();

private:

    void accept();

    /* 关闭连接 */
    void closeConn(int sock);

private:

    const int buf_size = 4096;


    std::string address_;           // 地址
    int port_;                      // 端口
    int listenfd_;                  // 监听服务端口
    int epfd_;                      // epoll监听端口
    sockaddr_in *local_address;     // 本地地址
    bool isQuit_;                   // 是否退出
    int OpenMax_;                   // epoll最大连接数
    epoll_event* eventArray_;       // epoll返回数组

    Rtsp* rtsp_;                    // RTSP解析器

};





#endif //RTSP_TCP_H




