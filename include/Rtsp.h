//
// Created by swagger on 2021/6/20.
//

#ifndef RTSP_RTSP_H
#define RTSP_RTSP_H

#include <iostream>
#include <map>

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
        DESCRIBE,
        OPTIONS,
        PLAY,
        PAUSE,
        SETUP,
        TEARDOWN,
        RECORD,
        ERROR = -1,
        CONTINUE,
        END,
    };

    typedef std::pair<std::string, std::string> Pair;

    Rtsp(int fd);

    ~Rtsp();

    void accept();

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

private:

    const int buf_size = 4096;

    char* buf_;             // 缓冲区
    int fd_;                // 套接字fd
    std::string version_;   // RTSP协议版本
    std::string request_;   // RTSP请求
    Options option_;    // 当前RTSP指

    int decode_index_;      // 解析行进度
    int recv_len_;          // 收到字符串长度

    std::map<std::string, std::string> headers_;    // 头部字段

};

#endif //RTSP_RTSP_H
