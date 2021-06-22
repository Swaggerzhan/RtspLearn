//
// Created by swagger on 2021/6/22.
//


#include "../include/EventLoop.h"
#include "../include/channel.h"
#include "../include/BlockQueue.h"
#include <iostream>
#include <cerrno>
#include <cstring>

using std::endl;
using std::cout;
using std::cerr;
using std::string;



const int EventLoop::kOpenMax = 10000;


EventLoop::EventLoop(int listenfd)
:   isQuit_(false),
    listenChannel_(new Channel(listenfd)),
    queue_(new BlockQueue(kOpenMax))
{
    epfd_ = epoll_create(5);
    eventArray_ = new Event[kOpenMax];
    Event event;        // 临时
    event.data.ptr = &*listenChannel_;
    event.events = EPOLLIN;     // 只需要监听接收事件
    epoll_ctl(epfd_, EPOLL_CTL_ADD, listenChannel_->getfd(), &event);

    cout << "EventLoop init OK !! " << endl;
}






EventLoop::~EventLoop() {
    delete eventArray_;
}




