//
// Created by swagger on 2021/6/22.
//


#include "../include/Mutex.h"

Mutex::Mutex()
:   mutex_()
{
    /* 互斥锁初始化 */
    pthread_mutex_init(&mutex_, nullptr);
    // TODO:   申请成功判断
}

Mutex::~Mutex() {
    /* 摧毁互斥锁 */
    pthread_mutex_destroy(&mutex_);
}

void Mutex::lock() {
    pthread_mutex_lock(&mutex_);
}

void Mutex::unlock() {
    pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* Mutex::getMutex() {
    return &mutex_;
}


MutexLockGuard::MutexLockGuard(Mutex &mutex)
:   mutex_(mutex)
{
    mutex_.lock();
}

MutexLockGuard::~MutexLockGuard() {
    mutex_.unlock();
}

Condition::Condition(Mutex &mutex)
:   mutex_(mutex)
{
    pthread_cond_init(&cond_, nullptr);
}

void Condition::wait() {
    pthread_cond_wait(&cond_, mutex_.getMutex());
}

void Condition::notify() {
    pthread_cond_signal(&cond_);
}

void Condition::notifyAll() {
    pthread_cond_broadcast(&cond_);
}