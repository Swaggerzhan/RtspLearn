//
// Created by swagger on 2021/6/21.
//


#include "../include/Buffer.h"
#include <cstring>


Buffer::Buffer(int bufSize)
:   bufSize_(bufSize)
{
    buf_ = new char[bufSize_];
    reSetBuf();
}


Buffer::~Buffer() {
    delete buf_;
}

void Buffer::reSetBuf() {
    memset(buf_, '\0', bufSize_);
}
/* 初始指针 */
char* Buffer::begin() {
    return buf_;
}
/* 当前可写指针 */
char* Buffer::curPoint() {
    return buf_ + curSize_;
}
/* 结尾指针 */
char* Buffer::end() {
    return buf_ + bufSize_;
}

