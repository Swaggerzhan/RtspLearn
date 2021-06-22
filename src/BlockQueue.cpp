//
// Created by swagger on 2021/6/22.
//



#include "../include/BlockQueue.h"
#include "../include/channel.h"
#include "../include/Mutex.h"

BlockQueue::BlockQueue(int size)
:   size_(size),
    queue_(),
    mutex_(new Mutex)
{

}


BlockQueue::~BlockQueue() {
    delete mutex_;
}


bool BlockQueue::push(Channel *channel) {
    MutexLockGuard(*mutex_);
    if ( queue_.size() >= size_ )
        return false;
    queue_.push(channel);
    return true;
}

Channel* BlockQueue::pop() {
    MutexLockGuard(*mutex_);
    if ( queue_.empty() )
        return nullptr;
    Channel* ret = queue_.front();
    queue_.pop();
    return ret;
}







