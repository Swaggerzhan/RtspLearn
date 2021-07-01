//
// Created by Swagger on 2021/7/1.
//

#include "../include/RTSP2.h"
#include "../include/channel.h"
#include "../include/Buffer.h"
#include <cstring>
#include <stack>

using std::stack;
using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;


RTSP::RTSP() {

}


RTSP::~RTSP() {

}


void RTSP::Process() {

    /* 解析并得到解析状态，通过状态来判断 */
    Options parserStatus = parser();

    // TODO: 回复报文等


}


RTSP::Options RTSP::parser() {

    Buffer* buffer = channel_->getRecvBuffer();
    /* 获取每行入口 */
    vector<char*> rowEntry = getRawDataEntry(buffer);
    if ( rowEntry.empty() ) //
        return ERROR;

    if (!parser_request_line(rowEntry[0])) // 首行为请求行
        return ERROR;

    parser_header(rowEntry);


}


bool RTSP::parser_header(std::vector<char*> list) {
    for (int i=1; i<list.size(); i++){
        int line_len = strlen(list[i]);
        char* left = list[i];
        char* right = nullptr;
        for (int colon_index=0; colon_index < line_len; colon_index++){
            if ( left[colon_index] == ':' ) {
                left[colon_index] = '\0';
                right = left + colon_index + 1;
                break;
            }
        }
        if (!left || !right){
            /// for debug
            cerr << "parser_header() Error! " << endl;
            return false;
        }
        // 需要考虑  ':' 和 ': '两种格式
        if ( *right == ' ')
            right ++;
        // 添加map中
        headers_.insert(header(left, right));
    }
    return true;
}


bool RTSP::parser_request_line(char *data) {
    if ( !data )
        return false;
    int len = strlen(data);
    stack<char*> s;
    char* row = data;
    for (int i=0; i<len; i++ ){
        if (data[i] == ' ' || data[i] == '\t'){
            data[i] = '\0';
            s.push(row);
            row = data + i + 1;
        }
    }
    char* version = row;            // 最后没有被推入的为version
    char* url = s.top(); s.pop();   // 栈顶是url
    char* option = s.top();         // 之后是第一个入栈的请求指令
    if ( !version || !url || !option){
        /// for debug
        cerr << "version url option get Error!" << endl;
        return false;
    }
    version_ = string(version);
    url_ = string(url);
    optionsString_ = string(option);
    options_ = stringOption2Enum(); // 将optionsString转换为option
    if (options_ == ERROR ){
        /// for debug
        cerr << "stringOption2Enum() Error! " << endl;
        return false;
    }
    return true;
}


RTSP::Options RTSP::stringOption2Enum() {

    if (strncasecmp(optionsString_.c_str(), "DESCRIBE", 8) == 0 ){
        return DESCRIBE;
    }
    if (strncasecmp(optionsString_.c_str(), "OPTIONS", 7) == 0 ){
        return OPTIONS;
    }
    if (strncasecmp(optionsString_.c_str(), "PLAY", 4) == 0 ){
        return PLAY;
    }
    if (strncasecmp(optionsString_.c_str(), "PAUSE", 5) == 0 ){
        return PAUSE;
    }
    if (strncasecmp(optionsString_.c_str(), "SETUP", 5) == 0 ){
        return SETUP;
    }
    if (strncasecmp(optionsString_.c_str(), "TEARDOWN", 8) == 0 ){
        return TEARDOWN;
    }
    if (strncasecmp(optionsString_.c_str(), "RECORD", 6) == 0 ){
        return RECORD;
    }
    return ERROR;


}

vector<char*> RTSP::getRawDataEntry(Buffer* buffer) {
    vector<char*> ret;
    char* data = buffer->begin();
    char* row = data;
    for (int i=0; i<buffer->size()-2; i++){
        if (data[i] == '\r' && data[i+1] == '\n'){
            data[i] = '\0';
            data[i + 1] = '\0';
            ret.push_back(row);
            row = data + i + 2;
            if ( *row == '\r' ) // 最后一行
                break;
        }
    }
    return ret;
}

/// for debug
void RTSP::printHeaders() {
    auto iter = headers_.begin();
    for (; iter != headers_.end(); iter ++){
        cout << iter->first << " : " << iter->second << endl;
    }
}



