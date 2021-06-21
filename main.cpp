#include <iostream>
#include "include/Tcp.h"
#include <cstring>

int main() {


    Tcp tcp("0.0.0.0", 8888);
    tcp.run();


//    std::string options = "OPTIONS";
//    if (strncasecmp(options.c_str(), "OPTIONS", 7) == 0)
//        std::cout << "got" << std::endl;

}
