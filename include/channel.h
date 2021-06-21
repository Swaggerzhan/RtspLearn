//
// Created by swagger on 2021/6/21.
//

#ifndef RTSP_CHANNEL_H
#define RTSP_CHANNEL_H
#include <memory>

class Buffer;

class Channel{
public:

    Channel();

    ~Channel();

    /* 设置fd，并且初始化channel */
    void setfd(int fd);

    bool send();

    bool recv();

    void closeConn();

public:

    const static int kBufSize;

private:

    int recvCurLen_;        // 当前收到的长度

    int writeTotalLen_;     // 写入总长度
    int writeCurLen_;       // 写入当前长度

    int fd_;                // 套接字fd
    //char* buf_;             // 接收缓冲区

    std::unique_ptr<Buffer> buf_;   // 接收缓冲区
    std::unique_ptr<Buffer> buf2_;  // 发送缓冲区

    ////////////////////////////////////////////////////////////////
    //// for debug
    bool isClosed_;         // 是否被关闭


};

#endif //RTSP_CHANNEL_H
