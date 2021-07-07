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
#include "functions.h"

std::string URL;  //域名
void forwardQuery(char *recvBuf, sockaddr_in receive_in);   //将查询转发到本地DNS服务器

int main(){
    //WSA init
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0) {
        return -1;
    }
    DNSStore store;
    store.initLocalTable(PATH);
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
    while(1){
        int len_rece = sizeof(receive_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        int rec_len;
        rec_len= recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *)&receive_in, &len_rece);

        if(rec_len != -1 && rec_len != 0) {
            char* tmp_ptr=rece_buff;
            DNS_HEADER* header=MessageDealer::getDNSHeader(tmp_ptr);
            DNS_QUERY * query=MessageDealer::getDNSQuery(tmp_ptr);
            if(query->type!=1&&query->type!=28) {// type not A & AAAA

            }
            else {
                URL = MessageDealer::getHostName(tmp_ptr); // 读取域名
                std::string ip = store.getStoredIpByDomain(URL);   //查看是否在本地表中
                if(ip == ""){
                    forwardQuery(tmp_ptr, receive_in);
                }
                else if (ip=="nigeiwoligiaogiao") {
                    break; // ********************************
                }

                else{
                    char send_buf[MAX_BUFFER_SIZE];
                    unsigned short *pID=(unsigned short*)malloc(sizeof(unsigned short*));
                    memcpy(pID, rece_buff, sizeof(unsigned short));
                    memcpy(send_buf,rece_buff,rec_len);
                    unsigned short nID=11;//***********************
                    unsigned short AFlag=htons(0x8180); //1000 0001 1000 0000
                    memcpy(&send_buf[2], &AFlag, sizeof(unsigned short));
                    functions::printDNSInformation(nID,1,ip);
                    if (ip=="0.0.0.0") {
                        AFlag=htons(0x0000);
                        printf("**********  No such name!  **********\n");
                    }
                    else {
                        printf("**************  Have this name!  ****************\n");
                        AFlag=htons(0x0001);
                    }
                    memcpy(&send_buf[6], &AFlag, sizeof(unsigned short));

                    int length=0;
                    char answer[16];
                    unsigned short Name = htons(0xc00c);
                    memcpy(answer, &Name, sizeof(unsigned short));
                    length += sizeof(unsigned short);
                    unsigned short TypeA = htons(0x0001);
                    memcpy(answer + length, &TypeA, sizeof(unsigned short));
                    length += sizeof(unsigned short);

                    unsigned short ClassA = htons(0x0001);
                    memcpy(answer + length, &ClassA, sizeof(unsigned short));
                    length += sizeof(unsigned short);

                    unsigned long timeLive = htonl(0x7b);
                    memcpy(answer + length, &timeLive, sizeof(unsigned long));
                    length += sizeof(unsigned long);

                    unsigned short ResourceDataLength = htons(0x0004);
                    memcpy(answer + length, &ResourceDataLength, sizeof(unsigned short));
                    length += sizeof(unsigned short);
                    char* Ip= const_cast<char *>(ip.c_str());
                    auto IP = (unsigned long)inet_addr(Ip);
                    memcpy(answer + length, &IP, sizeof(unsigned long));
                    length += sizeof(unsigned long);
                    length +=  rec_len;
                    memcpy(send_buf +  rec_len, answer, length);

                    int isSend;
                    isSend = sendto(localSoc, send_buf, length, 0, (SOCKADDR*)&receive_in, sizeof(receive_in));
                    if (!isSend) {
                        printf("send failed");
                    }
                }
            }
        }
    }

    return 0;
}

void forwardQuery(char *recvBuf, sockaddr_in receive_in){
    unsigned short* recv_ID;
    unsigned short send_ID;
    recv_ID = (unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(recv_ID, recvBuf, sizeof(unsigned short));    // 收到报文的ID（前2字节）
    //send_ID = htons(MessageDealer::getNewID(ntohs(*recv_ID), receive_in, FALSE));
}