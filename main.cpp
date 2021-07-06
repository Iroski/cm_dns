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
void forwardQuery(char *recvBuf, sockaddr_in reveice_in);   //将查询转发到本地DNS服务器

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
//    struct sockaddr_in {
//        short	sin_family;
//        u_short	sin_port;
//        struct in_addr	sin_addr;
//        char	sin_zero[8];
//    };
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
                URL = MessageDealer::getHostName(tmp_ptr); // 读取域名
                std::string ip = (new DNSStore)->getStoredIpByDomain(URL);   //查看是否在本地表中
                if(ip == ""){
                    forwardQuery(tmp_ptr, reveice_in);git
                }
                else if (ip=="nigeiwoligiaogiao") {
                    break; // ********************************
                }
                unsigned short AFlag=htons(0x8180);
                memccpy(s)
                else{
                    if (ip=="0.0.0.0") {
                        printf("**********No such name!**********");
                    }
                    else {

                    }
                }
            }
        }


    }

    return 0;
}

void forwardQuery(char *recvBuf, sockaddr_in reveice_in){
    unsigned short* recv_ID;
    unsigned short send_ID;
    recv_ID = (unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(recv_ID, recvBuf, sizeof(unsigned short));    // 收到报文的ID（前2字节）
    //send_ID = htons(MessageDealer::getNewID(ntohs(*recv_ID), reveice_in, FALSE));
}