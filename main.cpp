#include <iostream>
#include "include/Tcp.h"


int main() {


    Tcp tcp("0.0.0.0", 8888);
    tcp.run();

}
