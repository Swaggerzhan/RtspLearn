//
// Created by swagger on 2021/6/20.
//

#include "../include/Rtsp.h"
#include <unistd.h>
#include <cstring>
#include <iostream>


using std::cerr;
using std::cout;
using std::endl;
using std::string;

Rtsp::Rtsp(int fd)
:   fd_(fd)
{

    buf_ = new char[buf_size];
    decode_index_ = 0;
    recv_len_ = 0;

}

Rtsp::~Rtsp() {

    delete buf_;

}


void Rtsp::accept() {
    int len = ::read(fd_, buf_, buf_size);
    if ( len == 0 ){
        // 关闭操作
        return;
    }
    recv_len_ += len;
    parser_entry();
    PrintRequest();
}


bool Rtsp::find_space(char *data, int& first, int& second) {
    int len = strlen(data);
    bool flag = false;
    if ( !data )
        return false;
    for (int i=0; i<len; i ++){
        if ( data[i] == ' ' || data[i] == '\t' ){
            if (!flag){
                flag = true;
                first = i;
            }else{
                second = i;
                return true;
            }
        }
    }
    return false;
}

//////////////////////////////////////////////////
/////////// FOR DEBUG
void Rtsp::PrintRequest() {

    cout << "--------------------------------------------" << endl;
    cout << "操作: " << option_ << endl;
    cout << "地址: " << request_ << endl;
    cout << "版本: " << version_ << endl;

    auto Iter = headers_.begin();
    for (; Iter != headers_.end(); Iter ++){
        cout << Iter->first << ": " << Iter->second << endl;
    }


}

Rtsp::Options Rtsp::parser_header(char *target) {
    if (!target)
        return ERROR;
    if (*target == '\0')
        return END;

    int len = strlen(target);
    int pix = -1;
    for (int i=0; i<len; i++){
        if ( target[i] == ':' ){
            pix = i;
            break;
        }
    }
    if (pix <= 0)
        return ERROR;
    target[pix] = '\0';
    char* value = target + pix + 1;
    headers_.insert(Pair(target, value));
    return CONTINUE;
}


Rtsp::Options Rtsp::parser_request(char *target, ParserStatus &status) {
    if (status == ParserHeader)
        return ERROR;
    int first_space = 0;
    int second_space = 0;
    if ( !find_space(target, first_space, second_space) )
        return ERROR;

    char* option;
    char* url;
    char* version;
    option = target;
    url = target + first_space + 1;
    version = target + second_space + 1;
    target[first_space] = '\0';
    target[second_space] = '\0';
    version_ = string(version);
    request_ = string(url);
    Options ret = detectOptions(string(option));
    if ( ret == ERROR )
        return ERROR;
    status = ParserHeader;
    return ret;

}

Rtsp::Options Rtsp::detectOptions(const std::string& options) {

    if (strncasecmp(options.c_str(), "DESCRIBE", 8) == 0 ){
        return DESCRIBE;
    }
    if (strncasecmp(options.c_str(), "OPTIONS", 7) == 0 ){
        return OPTIONS;
    }
    if (strncasecmp(options.c_str(), "PLAY", 4) == 0 ){
        return PLAY;
    }
    if (strncasecmp(options.c_str(), "PAUSE", 5) == 0 ){
        return PAUSE;
    }
    if (strncasecmp(options.c_str(), "SETUP", 5) == 0 ){
        return SETUP;
    }
    if (strncasecmp(options.c_str(), "TEARDOWN", 8) == 0 ){
        return TEARDOWN;
    }
    if (strncasecmp(options.c_str(), "RECORD", 6) == 0 ){
        return RECORD;
    }
    return ERROR;

}


/* 确保在recv后进行调用 */
Rtsp::Options Rtsp::parser_entry() {
    LineStatus lineStatus = LineOK;
    ParserStatus parserStatus = ParserRequest; //开始时从请求行开始解析
    char *tmp = nullptr;
    while ( (lineStatus = parser_line(&tmp, decode_index_, recv_len_)) == LineOK ){
        switch ( parserStatus ){
            case ParserRequest: {
                /* 解析请求头 */
                option_ = parser_request(tmp, parserStatus);
                if ( option_ == ERROR )
                    return ERROR;
                break;
            }
            case ParserHeader: {
                Options ret = parser_header(tmp);
                if (ret == END)
                    return option_;
                if (ret == ERROR )
                    return ERROR;
                if (ret == CONTINUE )
                    break;
                cout << "error detect" << endl;
                return ERROR;
            }
        }
    }
    return ERROR;
}


Rtsp::LineStatus Rtsp::parser_line(char** begin, int &decode_index, int &len) {
    *begin = buf_ + decode_index;
    for (; decode_index < len; decode_index ++){
        if ( buf_[decode_index] == '\r' ){
            /* 不完整数据 */
            if ( decode_index + 1 >= len )
                return LineIncomplete;

            if ( buf_[decode_index + 1] == '\n' ){
                buf_[decode_index] = '\0';
                buf_[decode_index + 1] = '\0';
                decode_index += 2;
                return LineOK;
            }
        }else if (buf_[decode_index] == '\n' ){
            if (buf_[decode_index - 1] == '\r'){
                buf_[decode_index] = '\0';
                buf_[decode_index - 1] = '\0';
                decode_index ++;
                return LineOK;
            }else{
                return LineError;
            }
        }
    }
    return LineIncomplete;
}


