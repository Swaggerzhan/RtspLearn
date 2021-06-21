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

const static string RTSP_VERSION = "RTSP/1.0 ";
const static string OK = "200 OK\r\n";
const static string UNAUTHORIZED = "401 Unauthorized\r\n";
const static string NOTFOUND = "404 Not Found\r\n";
const static string INTERNAL = "500 Internal Server Error\r\n";

const static string SERVER = "Server: RTSPLearn 1.0.0\r\n";
const static string PUBLIC = "OPTIONS, DESCRIBE, PLAY, PAUSE, SETUP, ANNOUNCE, TEARDOWN\r\n";


const static string TEST_DESCRIBE_STRING = "RTSP/1.0 200 OK\r\n"
                                           "      CSeq: 2\r\n"
                                           "      Content-Type: application/sdp\r\n"
                                           "      Content-Length: 210 \r\n"
                                           "      m=video 0 RTP/AVP 96\r\n"
                                           "      a=control:streamid=0\r\n"
                                           "      a=range:npt=0-7.741000\r\n"
                                           "      a=length:npt=7.741000\r\n"
                                           "      a=rtpmap:96 MP4V-ES/5544\r\n"
                                           "      a=mimetype:string;\"video/MP4V-ES\"\r\n"
                                           "      a=AvgBitRate:integer;304018\n"
                                           "      a=StreamName:string;\"hinted video track\"\r\n"
                                           "      m=audio 0 RTP/AVP 97\r\n"
                                           "      a=control:streamid=1\r\n"
                                           "      a=range:npt=0-7.712000\r\n"
                                           "      a=length:npt=7.712000\r\n"
                                           "      a=rtpmap:97 mpeg4-generic/32000/2\r\n"
                                           "      a=mimetype:string;\"audio/mpeg4-generic\"\r\n"
                                           "      a=AvgBitRate:integer;65790\r\n"
                                           "      a=StreamName:string;\"hinted audio track\"\r\n\r\n";


static std::string gotRTSP_STATUS_CODE(int code){
    switch (code) {
        case 200: {
            return RTSP_VERSION;
        }
        case 401: {
            return UNAUTHORIZED;
        }
        case 404: {
            return NOTFOUND;
        }
        default: {
            return INTERNAL;
        }
    }
}


Rtsp::Rtsp(int fd)
:   fd_(fd),
    decode_index_(0),
    recv_len_(0)
{
    buf_ = new char[buf_size];
}

Rtsp::~Rtsp() {

    delete buf_;

}


bool Rtsp::accept() {
    int len = ::read(fd_, buf_, buf_size);
    if ( len == 0 ){
        // 关闭操作
        return false;
    }
    recv_len_ += len;
    return true;


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
    cout << "操作: " << optionString_ << endl;
    cout << "地址: " << request_ << endl;
    cout << "版本: " << version_ << endl;

    auto Iter = headers_.begin();
    for (; Iter != headers_.end(); Iter ++){
        cout << Iter->first << ": " << Iter->second << endl;
    }

}


void Rtsp::optionRespond() {
    respond_ += RTSP_VERSION;
    respond_ += OK;
    string cseq = "CSeq: " + headers_["CSeq"] + "\r\n";
    respond_ += cseq;
    respond_ += SERVER;
    respond_ += PUBLIC;
    respond_ += "\r\n";
    write();

}

void Rtsp::describeRespond() {
    respond_ += TEST_DESCRIBE_STRING;
    write();
}

bool Rtsp::write() {

    int len = ::write(fd_, respond_.c_str(), respond_.size());
    if ( len < respond_.size() ){
        cout << "write not incomplete!" << endl;
        return false;
    }
    return true;

}

void Rtsp::process() {

    bool ret = accept();
    if ( !ret )
        cout << "accept() error!" << endl;

    parser_entry();
    PrintRequest();
    switch ( option_ ) {
        case OPTIONS: {
            return optionRespond();
        }
        case DESCRIBE: {
            cout << "describe" << endl;
            describeRespond();
            break;
        }
        default: {
            break;
        }
    }


}


void Rtsp::reSet() {
    decode_index_ = 0;
    recv_len_ = 0;
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
    optionString_ = string(option);
    Options ret = detectOptions(optionString_);
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


