//
// Created by Swagger on 2021/7/1.
//

#ifndef RTSP_ACCEPTER_H
#define RTSP_ACCEPTER_H

#include <cassert>
#include <iostream>
#include <memory>

class Channel;
class EventLoop;

class Accepter{
public:
    Accepter(Channel* channel, EventLoop* eventLoop);
    ~Accepter();
    void accept();

private:
    // 外部传入进来后将由Accpeter进行管理
    std::unique_ptr<Channel> channel_;          // 监听套接字
    EventLoop* eventLoop_;                      // 所属EventLoop

};



#endif //RTSP_ACCEPTER_H
