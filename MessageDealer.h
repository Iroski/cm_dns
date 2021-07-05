//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_MESSAGEDEALER_H
#define CM_DNS_CPP_MESSAGEDEALER_H
#include "define.h"
#include <winsock2.h>
#include <string>
#include <iostream>
class MessageDealer {
    public:
        static DNS_HEADER* getDNSHeader(char* buff);
        static DNS_QUERY * getDNSQuery(char buff[]);
        static std::string getHostName(char* buff);
        static void printHeaderAll(DNS_HEADER* header);
        static void printQueryAll(DNS_QUERY* query);

};


#endif //CM_DNS_CPP_MESSAGEDEALER_H
