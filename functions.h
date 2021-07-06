//
// Created by 1 on 2021/7/6.
//

#ifndef CM_DNS_CPP_FUNCTIONS_H
#define CM_DNS_CPP_FUNCTIONS_H


#include <string>
#include <winsock.h>
#include "DetailedLogDealer.h"

class functions {
    public:
        static void printDNSInformation(unsigned short ID, int find, std::string ip);
        static void forwardQuery(char *recvBuf, sockaddr_in reveice_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len, int debugMode);
        static void sendingBack(char *rece_buff, std::string ip, sockaddr_in receive_in, SOCKET localSoc, int rec_len);
};


#endif //CM_DNS_CPP_FUNCTIONS_H
