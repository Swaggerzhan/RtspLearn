//
// Created by Swagger on 2021/7/1.
//

#include "../include/Accepter.h"
#include "../include/channel.h"
#include "../include/EventLoop.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using std::endl;
using std::cout;
using std::cerr;


Accepter::Accepter(Channel *channel, EventLoop *eventLoop)
:   channel_(channel),
    eventLoop_(eventLoop)
{


}


Accepter::~Accepter() {

}


void Accepter::accept() {

    sockaddr_in remote_addr{};
    socklen_t remote_addr_sz;
    int sock = ::accept(channel_->getfd(),
                       (sockaddr*)&remote_addr, &remote_addr_sz);
    if ( sock < 0 ){
        cerr << "accept() Error! " << endl;
        return;
    }
    Channel* channel = new Channel(sock);
    // 添加到EventLoop中，非阻塞IO和阻塞IO区别较大！
    eventLoop_->addInLoop(channel);

}



