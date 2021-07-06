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
int ID_COUNT;
IDTransform IDTransTable[ID_AMOUNT];	//ID转换表
void forwardQuery(char *recvBuf, sockaddr_in receive_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len);   //将查询转发到本地DNS服务器

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

        if(rec_len != -1 && rec_len != 0){
            char* tmp_ptr=rece_buff;
            DNS_HEADER* header=MessageDealer::getDNSHeader(tmp_ptr);
            DNS_QUERY * query=MessageDealer::getDNSQuery(tmp_ptr);
            if(query->type!=1&&query->type!=28){// type not A & AAAA

            }else{
                URL = MessageDealer::getHostName(tmp_ptr); // 读取域名
                std::string ip = store.getStoredIpByDomain(URL);   //查看是否在本地表中
                if(ip == ""){
                    forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len);
                }
                else if (ip=="nigeiwoligiaogiao") {
                    break; // ********************************
                }
                else{

                }
            }
        }


    }

    return 0;
}

void forwardQuery(char *recvBuf, sockaddr_in reveice_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len){
    unsigned short* recv_ID;
    unsigned short send_ID;
    recv_ID = (unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(recv_ID, recvBuf, sizeof(unsigned short));    // 收到报文的ID（前2字节）
    send_ID = htons(MessageDealer::getNewID(ntohs(*recv_ID), reveice_in, FALSE));
    //PrintInfo(ntohs(send_ID), not_find);
    memcpy(recvBuf, &send_ID, sizeof(unsigned short));
    int send_len = sendto(externSoc, recvBuf, len, 0, (struct sockaddr *)&server_in, sizeof(server_in));
    free(recv_ID);

    DNS_HEADER *header = MessageDealer::getDNSHeader(recvBuf);
    DNS_QUERY *query = MessageDealer::getDNSQuery(recvBuf);
    std::cout << send_len << std::endl;
    MessageDealer::printQueryAll(query);
    MessageDealer::printHeaderAll(header);
    std::cout << "send end" << std::endl;

    clock_t start, stop; //定时
    double duration = 0;
    start = clock();
    int size = sizeof(server_in);
    int recv_len = recvfrom(externSoc, recvBuf, MAX_BUFFER_SIZE, 0, nullptr, nullptr); //接受从远端发回的信息

    if (recv_len != -1 && recv_len != 0) {
        char *tmp_ptr = recvBuf;
        DNS_HEADER *header = MessageDealer::getDNSHeader(tmp_ptr);
        DNS_QUERY *query = MessageDealer::getDNSQuery(tmp_ptr);
        std::cout << recv_len << std::endl;
        MessageDealer::printQueryAll(query);
        MessageDealer::printHeaderAll(header);
        std::cout << "receive end" << std::endl;
    }

    //ID转换
    memcpy(recv_ID, recvBuf, sizeof(unsigned short)); //报文前两字节为ID
    unsigned short oID = htons(IDTransTable[ntohs(*recv_ID)].oldID);  // 得到old id 发回给receive
    memcpy(recvBuf, &oID, sizeof(unsigned short));
    sockaddr_in client = IDTransTable[ntohs(*recv_ID)].client;
    send_len = sendto(localSoc, recvBuf, recv_len, 0, (SOCKADDR*)&client, sizeof(client));
    printf("%d\n", send_len);/////////////////////////////////
    if (send_len != -1 && send_len != 0) {
        char *tmp_ptr = recvBuf;
        DNS_HEADER *header = MessageDealer::getDNSHeader(tmp_ptr);
        DNS_QUERY *query = MessageDealer::getDNSQuery(tmp_ptr);
        std::cout << recv_len << std::endl;
        MessageDealer::printQueryAll(query);
        MessageDealer::printHeaderAll(header);
        std::cout << "receive end" << std::endl;
    }
    free(recv_ID); //释放动态分配的内存
}