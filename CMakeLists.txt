cmake_minimum_required(VERSION 2.8)
project(RTSP)

set(CMAKE_CXX_STANDARD 14)

add_executable(RTSP main.cpp include/Tcp.h src/Tcp.cpp include/Rtsp.h src/Rtsp.cpp include/channel.h src/channel.cpp include/Buffer.h src/Buffer.cpp)