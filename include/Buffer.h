//
// Created by swagger on 2021/6/21.
//

#ifndef RTSP_BUFFER_H
#define RTSP_BUFFER_H

/* 不是线程安全 */
class Buffer{
public:

    Buffer(int bufSize);

    ~Buffer();

    void reSetBuf();

    /* 拿到当前指向指针 */
    char* curPoint();

    /* 拿到开始指针 */
    char* begin();

    /* 拿到结尾指针 */
    char* end();

    /* 获取缓冲区大小 */
    int size();

    /**
     * 通过curPoint拿到指针后需要将长度通过addCurSize来重新确定长度
     * */
    inline char* addCurSize(int size){
        if (curSize_ + size >= bufSize_ )
            return end();
        curSize_ += size;
        return curPoint();
    }

private:

    char* buf_;             // 缓冲区
    int bufSize_;           // 缓冲区长度
    int curSize_;           // 当前写到的进度

};


#endif //RTSP_BUFFER_H
