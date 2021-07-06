#include <bits/stdc++.h>
#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <process.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll

#include "MessageDealer.h"
#include "define.h"
#include "DNSStore.h"
std::string URL;  //域名
int main() {
//    char* str="24098c006c21103d000000ffb00239ab";
//   std::cout<<MessageDealer::charToIpv6(str)<<std::endl;
//
//    exit(1);
    //WSA init
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0) {
        return -1;
    }

    //创建套接字
    SOCKET externSoc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKET localSoc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (externSoc == INVALID_SOCKET || localSoc == INVALID_SOCKET) {
        printf("socket error !");
        return -1;
    }

    struct sockaddr_in extern_in, local_in, receive_in, server_in;
    extern_in.sin_family = AF_INET;
    extern_in.sin_port = htons(EXTERN_PORT);
    extern_in.sin_addr.s_addr = 0;
    local_in.sin_family = AF_INET;
    local_in.sin_port = htons(PORT);
    local_in.sin_addr.s_addr = inet_addr(LOCAL_DNS_ADDR);
    server_in.sin_family = AF_INET;
    server_in.sin_port = htons(PORT);
    server_in.sin_addr.s_addr = inet_addr(SERVER_DNS_ADDR);
    if (bind(localSoc, (LPSOCKADDR) &local_in, sizeof(local_in)) == SOCKET_ERROR) {
        printf("bind error !");
        exit(-1);
    } else {
        printf("bind socket success for test\n");
    }
    if (bind(externSoc, (LPSOCKADDR) &extern_in, sizeof(extern_in)) == SOCKET_ERROR) {
        printf("bind error !");
        exit(-1);
    } else {
        printf("bind socket success for test\n");
    }

    char rece_buff[MAX_BUFFER_SIZE];
    while (true) {
        int len_rece = sizeof(receive_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        int rec_len;
        rec_len = recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *) &receive_in, &len_rece);

        if (rec_len != -1 && rec_len != 0) {
            std::cout << rec_len << std::endl;
            unsigned short iSend;
            iSend = sendto(externSoc, rece_buff, rec_len, 0, (struct sockaddr *) &server_in, sizeof(server_in));
            char *tmp_ptr = rece_buff;
            DNS_HEADER *header = MessageDealer::getDNSHeader(tmp_ptr);
            DNS_QUERY *query = MessageDealer::getDNSQuery(tmp_ptr);
            std::cout << iSend << std::endl;
            MessageDealer::printQueryAll(query);
            MessageDealer::printHeaderAll(header);
            std::cout << "send end" << std::endl;
            clock_t start, stop; //定时
            double duration = 0;
            start = clock();
            rec_len = recvfrom(externSoc, rece_buff, sizeof(rece_buff), 0, nullptr, nullptr); //接受从远端发回的信息

            tmp_ptr = rece_buff;
            memset(rece_buff, 0, MAX_BUFFER_SIZE);
            header = MessageDealer::getDNSHeader(tmp_ptr);
            query = MessageDealer::getDNSQuery(tmp_ptr);
            //todo: sendto reciver
            while ((rec_len == 0) || (rec_len == SOCKET_ERROR)) {
                rec_len = recvfrom(externSoc, rece_buff, sizeof(rece_buff), 0, (SOCKADDR *) &receive_in, &len_rece);
                stop = clock();
                duration = (double) (stop - start) / CLK_TCK;
                if (duration > 5) {
                    printf("Long Time No Response From Server.\n");
                    rec_len = -1;
                    break;
                }
            }
            if (rec_len != -1 && rec_len != 0) {
                char *tmp_ptr = rece_buff;
                DNS_HEADER *header = MessageDealer::getDNSHeader(tmp_ptr);
                DNS_QUERY *query = MessageDealer::getDNSQuery(tmp_ptr);
                MessageDealer::printQueryAll(query);
                MessageDealer::printHeaderAll(header);
            }

        }


    }

    return 0;
}