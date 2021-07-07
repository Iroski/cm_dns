#include <bits/stdc++.h>
#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <process.h>
#include <cstring>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll

#include "MessageDealer.h"
#include "define.h"
#include "DNSStore.h"
#include "DetailedLogDealer.h"
int getState(char *state);
std::string URL;  //域名
int ID_COUNT;
IDTransform IDTransTable[ID_AMOUNT];
int main(int argc, char **argv) {

    char *mode = "-dd";
    char *server_ip = "8.8.8.8";
    char *file_path = "dnsdelay.txt";
    if (argc >= 1)
        mode = argv[1]; //0:-d  1:-dd
    if (argc >= 2)
        server_ip = argv[2];
    if (argc >= 3)
        file_path = argv[3];
    int debug_mode=getState(mode);

//    char* str="24098c006c21103d000000ffb00239ab";
//    char tmp_str[40];//=new char;
//   std::cout<<MessageDealer::charToIpv6(str)<<std::endl;
//    char result_char [1024];
//    inet_ntop(AF_INET6,str,result_char,1024);
//    std::cout<<result_char<<std::endl;
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
    extern_in.sin_addr.s_addr = ADDR_ANY;
    local_in.sin_family = AF_INET;
    local_in.sin_port = htons(PORT);
    local_in.sin_addr.s_addr = inet_addr(LOCAL_DNS_ADDR);
    server_in.sin_family = AF_INET;
    server_in.sin_port = htons(PORT);
    server_in.sin_addr.s_addr = inet_addr(server_ip);
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

    char rece_buff[1024];
    while (true) {
        int len_rece = sizeof(receive_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0
        int rec_len;
        rec_len = recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *) &receive_in, &len_rece);
        char *tmp_ptr = rece_buff;
        Message local_message=MessageDealer::messageInit(tmp_ptr,false);
        DetailedLogDealer::readLocalAddr(rec_len,receive_in);
        MessageDealer::printDetailedInfo(local_message);
        unsigned short send_len;

        //向外部发包
        send_len = sendto(externSoc, rece_buff, rec_len, 0, (struct sockaddr *) &server_in, sizeof(server_in));
        clock_t start, stop; //定时
        double duration = 0;
        start = clock();
        //接收到外部的包
        rec_len = recvfrom(externSoc, rece_buff, sizeof(rece_buff), 0, nullptr, nullptr); //接受从远端发回的信息
        tmp_ptr = rece_buff;
        if (rec_len != 0) {
            Message message = MessageDealer::messageInit(tmp_ptr, true);
                std::cout << "----------EXTERN RESPONSE---------" << std::endl;
                MessageDealer::printDetailedInfo(message);

        }
    }

    return 0;
}

int getState(char *state) {
    int len = strlen(state);
    if (memcmp(state, "-dd", len) == 0)
        return 1;
    else if (memcmp(state, "-d", len) == 0)
        return 0;
    else {
        std::cout << "wrong input" << std::endl;
        exit(-1);
    }
}