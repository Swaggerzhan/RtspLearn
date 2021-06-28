//
// Created by swagger on 2021/6/22.
//

#ifndef RTSP_MUTEX_H
#define RTSP_MUTEX_H

#include <pthread.h>



/* 对于互斥锁的简单封装 */
class Mutex{
public:

    Mutex();
    ~Mutex();
    void lock();
    void unlock();
    pthread_mutex_t* getMutex();

private:
    pthread_mutex_t mutex_;     // 互斥锁
};


/* 互斥锁RAII封装方式 */
class MutexLockGuard{
public:

    MutexLockGuard(Mutex& mutex);
    ~MutexLockGuard();

private:
    Mutex &mutex_;               // 互斥锁
};

/* 条件变量 */
class Condition{
    // TODO: 实现

public:
    Condition(Mutex &mutex);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t cond_;   // 条件变量
    Mutex &mutex_;          // 互斥量

};



#endif //RTSP_MUTEX_H
