//
// Created by swagger on 2021/6/20.
//

#ifndef RTSP_RTSP_H
#define RTSP_RTSP_H

#include <iostream>


class Rtsp{
public:

    enum Options{
        DESCRIBE,
        OPTIONS,
        PLAY,
        PAUSE,
        SETUP,
        TEARDOWN,
        RECORD
    };

    Rtsp(int fd);

    ~Rtsp();

    void accept();

private:

    /* 有限状态机写法 */
    //bool DecodeHeader(int &read_index, int &len, int &start_line);

    void parser_request_header(char* data);

    char* find_space(char* data);

    void PrintRequest();

private:

    const int buf_size = 4096;

    char* buf_;             // 缓冲区
    int fd_;                // 套接字fd
    std::string version_;   // RTSP协议版本
    std::string request_;   // RTSP请求
    std::string option_;    // 当前RTSP指



};

#endif //RTSP_RTSP_H
