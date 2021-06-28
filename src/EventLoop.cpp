//
// Created by swagger on 2021/6/22.
//


#include "../include/EventLoop.h"
#include "../include/channel.h"
#include "../include/BlockQueue.h"
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


void EventLoop::addInLoop(int fd) {
    Channel* channel = queue_->pop(); // 从队列中取出未使用的channel
    // for debug
    assert(channel->status());      // 检测管道是否未使用
    channel->setfd(fd);
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




