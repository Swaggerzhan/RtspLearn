//
// Created by swagger on 2021/6/20.
//

#include "../include/Tcp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include "../include/Rtsp.h"

using std::cerr;
using std::cout;
using std::string;
using std::endl;


Tcp::Tcp(string address, int port)
:   address_(std::move(address)),
    port_(port),
    OpenMax_(1000),
    isQuit_(false)
{

    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    local_address = new struct sockaddr_in;
    memset(local_address, 0, sizeof(*local_address));
    local_address->sin_family = AF_INET;
    local_address->sin_addr.s_addr = inet_addr(address_.c_str());
    local_address->sin_port = htons(port_);
    int ret = ::bind(listenfd_, (sockaddr*)local_address, sizeof(*local_address));
    if ( ret < 0 ){
        cerr << "bind() address Error! " << endl;
        exit(ret);
    }
    ret = listen(listenfd_, 5);
    if ( ret < 0 ){
        cerr << "listen() Error! " << endl;
        exit(ret);
    }

    epfd_ = epoll_create(5);
    if ( epfd_ < 0 ){
        cerr << "epoll_create() Error! " << endl;
        exit(epfd_);
    }

    /* 将监听端口添加到epoll中 */
    epoll_event event;
    event.data.fd = listenfd_;
    event.events = EPOLLIN;
    epoll_ctl(epfd_, EPOLL_CTL_ADD, listenfd_, &event);

    eventArray_ = new epoll_event[OpenMax_];
    cout << "Tcp init OK! " << endl;

}

void Tcp::run() {

    while ( !isQuit_ ){
        int ret = epoll_wait(epfd_, eventArray_, OpenMax_, 0);
        for (int i=0; i<ret; i++){
            int sock = eventArray_[i].data.fd;
            if ( sock == listenfd_ ){
                accept();
            }else{
                rtsp_->accept();
            }
        }
    }

}


void Tcp::closeConn(int sock) {

    close(sock);
    epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, nullptr);

}



void Tcp::accept() {
    sockaddr_in remote_addr{};
    socklen_t remote_addr_sz{};
    int sockfd = ::accept(listenfd_, (sockaddr*)&remote_addr, &remote_addr_sz);
    if ( sockfd < 0 ){
        cerr << "accept() Error!" << endl;
        return;
    }
    epoll_event event{};
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    epoll_ctl(epfd_, EPOLL_CTL_ADD, sockfd, &event);
    rtsp_ = new Rtsp(sockfd);

}

Tcp::~Tcp() {
    delete eventArray_;
}



