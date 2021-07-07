//
// Created by 1 on 2021/7/6.
//

#ifndef CM_DNS_CPP_FUNCTIONS_H
#define CM_DNS_CPP_FUNCTIONS_H


#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <WinSock2.h>
#include "DetailedLogDealer.h"
#include "SimpleLogDealer.h"
#include "define.h"
#include "MessageDealer.h"
#include "DNSStore.h"
#include "DNSStore.h"

class functions {
    public:
        static void printDNSInformation(unsigned short ID, int find, std::string ip);
        static void forwardQuery(char *recvBuf, sockaddr_in reveice_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len, int debugMode);
        static void sendingBack(char *rece_buff, std::string ip, sockaddr_in receive_in, SOCKET localSoc, int rec_len,std::string type,int debug_mode);
        static void sendBackPTR(char *rece_buff,sockaddr_in receive_in, SOCKET localSoc);
        void str_split(const std::string &str, const std::string &sign, std::vector<std::string> &results);
        EM_IP_TYPE Check_IP_V6(std::vector<std::string> vecIpSection);
        EM_IP_TYPE Check_IP(std::string strIP);
        std::string Get_Type_Name(EM_IP_TYPE emType);
        EM_IP_TYPE Check_IP_V4(std::vector<std::string> vecIpSection);
};


#endif //CM_DNS_CPP_FUNCTIONS_H
