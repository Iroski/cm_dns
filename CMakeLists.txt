cmake_minimum_required(VERSION 3.19)
project(cm_dns_cpp)

set(CMAKE_CXX_STANDARD 14)
link_libraries(ws2_32 wsock32)
add_executable(cm_dns_cpp main.cpp define.h DNSStore.cpp DNSStore.h)