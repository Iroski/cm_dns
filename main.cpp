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
int main(){
    //WSA init
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return -1;
    }

    //创建套接字
    SOCKET serveSoc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKET localSoc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serveSoc == INVALID_SOCKET|| localSoc==INVALID_SOCKET) {
        printf("socket error !");
        return -1;
    }
    int unBlock=-1;
    ioctlsocket(serveSoc,FIONBIO,(u_long FAR*)&unBlock);
    ioctlsocket(localSoc,FIONBIO,(u_long FAR*)&unBlock);
    struct sockaddr_in serve_in, local_in,reveice_in;
    serve_in.sin_family = AF_INET;
    serve_in.sin_port = htons(PORT);
    serve_in.sin_addr.s_addr = inet_addr(SERVE_DNS_ADDR);
    local_in.sin_family = AF_INET;
    local_in.sin_port = htons(PORT);
    local_in.sin_addr.s_addr = inet_addr(LOCAL_DNS_ADDR);
    if (bind(localSoc, (LPSOCKADDR)&local_in, sizeof(local_in)) == SOCKET_ERROR)
    {
        printf("bind error !");
        exit(-1);
    }else
        printf("bind socket success\n");

    char rece_buff[MAX_BUFFER_SIZE];
    while(1){
        int len_rece = sizeof(reveice_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        int rec_len;
        rec_len= recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *)&reveice_in, &len_rece);

        if(rec_len != -1 && rec_len != 0){
            char* tmp_ptr=rece_buff;
            DNS_HEADER* header=MessageDealer::getDNSHeader(tmp_ptr);
            DNS_QUERY * query=MessageDealer::getDNSQuery(tmp_ptr);
            if(query->type!=1&&query->type!=28){// type not A & AAAA

            }else{

            }
        }


    }

    return 0;
}