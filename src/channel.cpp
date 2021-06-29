//
// Created by swagger on 2021/6/21.
//

#include "../include/channel.h"
#include <unistd.h>
#include "../include/Buffer.h"
#include "../include/Rtsp.h"
#include <cassert>
#include <cerrno>


const int Channel::kBufSize = 4096;

Channel::Channel()
:   buf_(new Buffer(kBufSize)),
    buf2_(new Buffer(kBufSize)),
    rtsp_(new Rtsp),
    isClosed_(true)     // 初始化的时候没有fd，后续由setfd进行设置
{

}


Channel::Channel(int fd)
:   fd_(fd),
    buf_(new Buffer(kBufSize)),
    buf2_(new Buffer(kBufSize)),
    isClosed_(false)    // 初始化就有fd，直接非关闭状态
{}


Channel::~Channel() {

}

int Channel::getfd() const {
    return fd_;
}

/**
 * @param 需要设置的fd
 * 会将频道状态设置为使用状态 isClosed = false
 * */
void Channel::setfd(int fd) {
    assert(isClosed_);
    fd_ = fd;
    isClosed_ = false; // 管道已被使用
}

///////////////////////////////////////////////////////////////////////////////////////////
/// 非阻塞型函数，暂定
/** send函数开放，可以由IO线程调用，也可以由处理线程尝试调用
 *  当处理线程调用后返回失败将交由IO线程进行处理
 * */
bool Channel::send2() {
    assert(!isClosed_);
    while (writeCurLen_ < writeTotalLen_){
        int roundLen = ::write(fd_, buf2_->curPoint(), writeTotalLen_);
        /* 写入堵塞 */
        if (roundLen == -1){
            //TODO: 通过errno判断

            /* 写入阻塞了，需要重新加入到Epoll中，并且改变监听状态为EPOLLOUT */
            if ( errno == EAGAIN || errno == EWOULDBLOCK ){
                // TODO: 加入到Epoll循环
                return false;
            }else { /* 出错！ */
                // TODO: 出错处理
            }
        }
        buf2_->addCurSize(roundLen);    // 记录buffer中的当前写入进度
        writeCurLen_ += roundLen;       // 记录writeCurLne
    }
    return true;

}


bool Channel::recv2() {
    assert(!isClosed_);
    while ( true ){
        int len = ::read(fd_, buf_->curPoint(), kBufSize);
        if ( len < 0 ){
            // TODO: 通过errno判断
            /* 读取到尾部了，非阻塞套接字正常返回 */
            if ( errno == EAGAIN || errno == EWOULDBLOCK ){
                return true; // 将true返回给上层
            }
        }else if (len == 0){ // 关闭事件
            // TODO: 关闭事件
            return false; // 暂时不处理
        }else{ // len > 0
            buf_->addCurSize(len); // 增加读取长度
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////



/* 关闭套接字，尽量不要直接调用，应该由上层协议来调用 */
void Channel::closeConn() {
    assert(!isClosed_);
    close(fd_);
    isClosed_ = true;
    // TODO: 更新其在Epoll中的状态
    // TODO: 上层channel拥有者应该将channel重新加入到队列中去
}








