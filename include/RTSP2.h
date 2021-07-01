//
// Created by Swagger on 2021/7/1.
//

#ifndef RTSP_RTSP2_H
#define RTSP_RTSP2_H

#include <iostream>
#include <vector>
#include <map>

class Channel;
class Buffer;

class RTSP{
public:

    typedef std::pair<std::string, std::string> header;

    enum Options{
        DESCRIBE,           // DESCRIBE请求
        OPTIONS,            // OPTIONS请求
        PLAY,               // 播放请求
        PAUSE,              // PAUSE暂停请求
        SETUP,              // SETUP请求
        TEARDOWN,           // TEARDOWN请求
        RECORD,             // RECORD请求
        ERROR = -1,         // 错误
        CONTINUE,           // 请求需要继续
        END,                // 请求结尾
    };

    RTSP();
    ~RTSP();

    void Process();

private:

    Options stringOption2Enum();


    /* 解析请求 */
    Options parser();
    /* 获取每行入口，主要是将\r\n替换为\0\0 */
    std::vector<char*> getRawDataEntry(Buffer* buffer);
    /* 解析第一行 */
    bool parser_request_line(char* data);
    bool parser_header(std::vector<char*> list);


    /////////////////////////////////////////////////////
    /// for debug
    void printHeaders();
    /////////////////////////////////////////////////////


private:

    Channel* channel_;

    Options options_;                               // 当前请求指令
    std::string optionsString_;                     // for debug
    std::string version_;                           // 版本
    std::string url_;                               // RTSP请求
    std::map<std::string, std::string> headers_;    // 请求头字段

};




#endif //RTSP_RTSP2_H
