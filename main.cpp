#include <iostream>
#include "include/EventLoop.h"
#include "include/Socket.h"
#include <cstring>


using std::string;

int main() {


//    Tcp tcp("0.0.0.0", 8888);
//    tcp.run();
    Socket s( string("0.0.0.0"), 9999 );
    EventLoop loop(s.getfd());
    loop.loop();    // 启动主循环



}
