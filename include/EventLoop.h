//
// Created by swagger on 2021/6/22.
//

#ifndef RTSP_EVENTLOOP_H
#define RTSP_EVENTLOOP_H

#include <sys/epoll.h>
#include <memory>
#include <queue>


class BlockQueue;
class Channel;
class Accepter;

class EventLoop{
public:

    typedef epoll_event Event;      // 事件
    EventLoop(int listenfd);
    ~EventLoop();

    /* 主循环 */
    void loop();
    void poll();
    void updateInLoop(Channel* channel);            // 管道更新
    void removeInLoop(Channel* channel);            // 管道删除
    void addInLoop(Channel* channel);               // 管道增加


private:

    const static int kOpenMax;                  // 支持的最大套接字

    int epfd_;                                  // Epoll套接字
    std::unique_ptr<Accepter> accepter_;        // 接受套接字
    std::unique_ptr<BlockQueue> queue_;         // 空间频道队列
    Event* eventArray_;                         // 返回监听套接字的数组
    std::queue<Channel*> activeChannel_;        // 活跃的频道

    bool isQuit_;                               // 主要循环退出情况


};



#endif //RTSP_EVENTLOOP_H
