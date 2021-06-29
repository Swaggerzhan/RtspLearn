//
// Created by swagger on 2021/6/20.
//

#ifndef RTSP_RTSP_H
#define RTSP_RTSP_H

#include <iostream>
#include <map>

//////////////////////////////////////////////////////
/// TODO:将read和write分离取出，read和write可以由外界调用


class Channel;

class Rtsp{
public:

    /* 有限状态机行状态 */
    enum LineStatus{
        LineOK,
        LineIncomplete,
        LineError,
    };

    /* 有限状态机解析状态 */
    enum ParserStatus{
        ParserRequest,
        ParserHeader
    };

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

    typedef std::pair<std::string, std::string> Pair;

    Rtsp(int fd);

    Rtsp();

    ~Rtsp();

    bool accept();

    void process();

    bool write();

private:


    /** 检测行状态
     * @param begin 解析结束后返回行开头
     * @param decode_index 解析进度
     * @param len 字符串总长度
     * return ERROR on fail
     * */
    LineStatus parser_line(char** begin, int& decode_index, int& len);

    /* 请求入口 */
    Options parser_entry();

    /* 解析请求头 */
    Options parser_request(char* target, ParserStatus& status);

    /* 字符串options方法转enum */
    static Options detectOptions(const std::string& options);

    /**
     * 解析请求头
     * @param target 需要解析的行
     * return ERROR on fail
     * */
    Options parser_header(char* target);


    /** 找到data中存在空格或者\t的index，通过first和second返回
     * return true on success false on fail
     *
     * @param *data 数据
     * @param first 第一个空格或者\t
     * @param second 第二个空格或者\t
     * */
    bool find_space(char* data, int& first, int& second);

    // FOR DEBUG
    void PrintRequest();

    /* 清空请求解析进度 */
    void reSet();

    void packetRespond();

    void optionRespond();
    void describeRespond();

private:

    const int buf_size = 4096;


    Channel* channel_;          // Rtsp所使用的管道

    char* buf_;                 // 缓冲区
    int fd_;                    // 套接字fd
    std::string version_;       // RTSP协议版本
    std::string request_;       // RTSP请求
    std::string optionString_;  // FOR DEBUG
    Options option_;            // 当前RTSP指令

    //////////////////////////////////////////////////////////////
    /// 每次请求都需要独立清空
    int decode_index_;      // 解析行进度
    int recv_len_;          // 收到字符串长度

    std::string respond_;   // 返回头
    //////////////////////////////////////////////////////////////

    std::map<std::string, std::string> headers_;    // 头部字段

};

#endif //RTSP_RTSP_H
