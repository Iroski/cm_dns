//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_MESSAGEDEALER_H
#define CM_DNS_CPP_MESSAGEDEALER_H
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <cstring>
#include <iostream>
#include "define.h"
class MessageDealer {
    public:
        static DNS_HEADER* getDNSHeader(char* buff);
        static DNS_QUERY * getDNSQuery(char buff[]);
        static std::string getHostName(char* buff);
        static void printHeaderAll(DNS_HEADER* header);
        static void printQueryAll(DNS_QUERY* query);
        static char* ipv4ToChar(const std::string& str);
        static char* ipv6ToChar(const std::string& str);
        static std::string charToIpv4(char* str);
        static std::string charToIpv6(char* str);
        static unsigned short getNewID(unsigned short recv_ID, sockaddr_in receive_in, BOOL processed);

};


#endif //CM_DNS_CPP_MESSAGEDEALER_H
