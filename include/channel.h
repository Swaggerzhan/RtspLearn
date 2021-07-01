//
// Created by swagger on 2021/6/21.
//

#ifndef RTSP_CHANNEL_H
#define RTSP_CHANNEL_H
#include <memory>

class Buffer;
class EventLoop;
class Rtsp;

class Channel{
public:

    Channel();
    Channel(int fd);

    ~Channel();

    /* 设置fd，并且初始化channel */
    void setfd(int fd);
    int getfd() const;

    bool send();
    bool recv();

    Rtsp* getRtspController();

    /////////////////////////////////////////////////////////
    /// 非阻塞
    bool send2();
    bool recv2();
    /////////////////////////////////////////////////////////
    void closeConn();

    /////////////////////////////////////////////////////////
    /// for debug
    bool status() { return isClosed_; }
    bool setClose() { isClosed_ = true; }
    /////////////////////////////////////////////////////////
    bool isOneShot() { return isOneShot_; }

public:

    const static int kBufSize;

private:
    /////////////////////////////////////////////////////////
    /// 非阻塞
    int recvCurLen_;                    // 当前收到的长度
    int writeTotalLen_;                 // 写入总长度
    int writeCurLen_;                   // 写入当前长度
    /////////////////////////////////////////////////////////

    int fd_;                            // 套接字fd
    bool isOneShot_;                    // EPOLL中的flag，为true需要重新加入epoll
    EventLoop* belongEventLoop_;        // 所属epoll
    std::unique_ptr<Rtsp> rtsp_;        // 所属rtsp解析器
    //char* buf_;                       // 接收缓冲区

    std::unique_ptr<Buffer> buf_;       // 接收缓冲区
    std::unique_ptr<Buffer> buf2_;      // 发送缓冲区

    ////////////////////////////////////////////////////////////////
    //// for debug
    bool isClosed_;         // 是否被关闭


};

#endif //RTSP_CHANNEL_H
