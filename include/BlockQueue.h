//
// Created by swagger on 2021/6/22.
//

#ifndef RTSP_BLOCKQUEUE_H
#define RTSP_BLOCKQUEUE_H


#include <queue>

class Mutex;

class Channel;

/* 队列，暂时不考虑泛型，线程安全 */
class BlockQueue{
public:

    BlockQueue(int size);
    ~BlockQueue();

    bool push(Channel* channel);
    Channel* pop();



private:

    int size_;                          // 队列长度
    std::queue<Channel*> queue_;        // 队列
    Mutex* mutex_;

};


#endif //RTSP_BLOCKQUEUE_H
