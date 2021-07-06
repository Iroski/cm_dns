//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_MESSAGEDEALER_H
#define CM_DNS_CPP_MESSAGEDEALER_H
#include "define.h"
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "define.h"
#include <ws2tcpip.h>
#include "Message.h"
class MessageDealer {
    public:
        static Message messageInit(char* ptr,bool isResponse);
        static DNS_HEADER* getDNSHeader(char* buff);
        static DNS_QUERY * getDNSQuery(char buff[]);
        static std::string getHostName(char* buff, char *domain_start_ptr);
        static void printHeaderDetailed(DNS_HEADER* header);
        static void printQueryDetailed(DNS_QUERY* query);
        static void printResponsesDetailed(const std::vector<DNS_RESPONSE>& responses);
        static void printDetailedInfo(Message message);
        static char* ipv4ToChar(const std::string& str);
        static char* ipv6ToChar(const std::string& str);
        static std::string charToIpv4(char* str);
        static std::string charToIpv6(char* str);
        static unsigned short* getNewID(unsigned short* recv_ID, sockaddr_in reveice_in, BOOL processed);
        static std::vector<DNS_RESPONSE> getAllResponses(char* ptr,int startPoint,int num);
        static DNS_RESPONSE getResponse(char* ptr, int* length,char * domain_start_ptr);

};


#endif //CM_DNS_CPP_MESSAGEDEALER_H
