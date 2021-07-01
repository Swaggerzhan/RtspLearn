//
// Created by swagger on 2021/6/22.
//


#include "../include/EventLoop.h"
#include "../include/channel.h"
#include "../include/BlockQueue.h"
#include "../include/Accepter.h"
#include "../include/Rtsp.h"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <cassert>


using std::endl;
using std::cout;
using std::cerr;
using std::string;

const int EventLoop::kOpenMax = 10000;


static int setnonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}


/**
 * 内部函数
 * @param epfd epoll fd
 * @param channel 需要加入到epfd中的管道
 *
 * */
static void addToEpoll(int epfd, Channel* channel){
    int flag;
    if (channel->isOneShot())
        flag = EPOLLIN | EPOLLET | EPOLLONESHOT;
    else
        flag = EPOLLIN | EPOLLET;
    EventLoop::Event event;
    event.events = flag;
    event.data.ptr = channel;
    epoll_ctl(epfd, EPOLL_CTL_ADD, channel->getfd(), &event);
}


static void updateToEpoll(int epfd, Channel* channel){
    int flag;
    if (channel->isOneShot())
        flag = EPOLLIN | EPOLLET | EPOLLONESHOT;
    else
        flag = EPOLLIN | EPOLLET;
    EventLoop::Event event;
    event.events = flag;
    event.data.ptr = channel;
    epoll_ctl(epfd, EPOLL_CTL_MOD, channel->getfd(), &event);
}

/**
 * @param epfd Epoll fd
 * @param channel 管道
 * 删除channel
 * */
static void removeToEpoll(int epfd, Channel* channel){
    epoll_ctl(epfd, EPOLL_CTL_DEL, channel->getfd(), nullptr);
}


EventLoop::EventLoop(int listenfd)
:   isQuit_(false),
    accepter_(new Accepter(new Channel, this)), // 初始化Accepter
    queue_(new BlockQueue(kOpenMax))
{
    /* 初始化Accepter监听套接字 */
    accepter_->getChannel()->setfd(listenfd);
    epfd_ = epoll_create(5);
    eventArray_ = new Event[kOpenMax];
    Event event;        // 临时
    event.data.ptr = accepter_->getChannel();
    event.events = EPOLLIN;     // 只需要监听接收事件
    epoll_ctl(epfd_, EPOLL_CTL_ADD, accepter_->getChannel()->getfd(), &event);
    cout << "EventLoop init OK !! " << endl;
}


EventLoop::~EventLoop() {
    delete eventArray_;
}

/* 由Accepter进行调用 */
void EventLoop::addInLoop(Channel* channel) {

    addToEpoll(epfd_, channel);     // 真正的添加到epoll中

}


void EventLoop::removeInLoop(Channel *channel) {
    // for debug
    assert(!channel->status());
    removeToEpoll(epfd_, channel);  // 删除
    channel->setClose();            // isClosed = true
    queue_->push(channel);          // 加入到队列中
}


void EventLoop::updateInLoop(Channel *channel) {
    // for debug
    assert(!channel->status());
    updateToEpoll(epfd_, channel);
}


void EventLoop::poll() {
    int ret = epoll_wait(epfd_, eventArray_, kOpenMax, 0);
    for (int i=0; i<ret; i++){
        Channel* channel = static_cast<Channel*>(eventArray_[i].data.ptr);
        if ( channel == accepter_->getChannel() ){ // 监听频道
            accepter_->accept(); // 接受请求
        } else {
            // if active Channel push them into activeChannel queue!
            activeChannel_.push(channel);
        }
    }
}


void EventLoop::loop() {
    while ( !isQuit_ ){
        poll();         // 抓取活跃频道

        while ( !activeChannel_.empty() ){
            Channel* channel = static_cast<Channel*>(activeChannel_.front());
            activeChannel_.pop();
            // TODO: 修改为线程池的运行方式
            Rtsp* rtsp = channel->getRtspController();
            rtsp->process();    // Rtsp解析入口，其中保存各种客户信息及状态
        }


    }
}




