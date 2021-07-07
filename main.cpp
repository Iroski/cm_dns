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
#include "DetailedLogDealer.h"

std::string URL;  //域名
int ID_COUNT;
IDTransform IDTransTable[ID_AMOUNT];    //ID转换表
int getState(char *state);
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
    while (1) {
//        std::cout<<"new round"<<std::endl;
        int len_rece = sizeof(receive_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        int rec_len;
        rec_len = recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *) &receive_in, &len_rece); //收到local
        if(rec_len!=-1&&rec_len!=0){
            std::string type;
            char *tmp_ptr = rece_buff;
            Message local_message=MessageDealer::messageInit(tmp_ptr,false);
            DetailedLogDealer::receiveLocalInit();
            DetailedLogDealer::readLocalAddr(rec_len,receive_in);
            MessageDealer::printDetailedInfo(local_message);
            DNS_QUERY *query = local_message.getQuery();

            if (query->type != "IPV4" && query->type != "IPV6") {// type not A & AAAA
                if(query->type=="PTR"){
                    char *tmp_ptr = rece_buff;
                    char send_buf[MAX_BUFFER_SIZE];
                    Message message=MessageDealer::messageInit(tmp_ptr,true);
                    MessageDealer::getDNSHeader(rece_buff);
                    DNS_QUERY *query=message.getQuery();
                    int len=query->headerAndQueryLength;
                    memcpy(send_buf, rece_buff, len);
                    char answer[16];
                    int length=0;
                    unsigned short Name = htons(0xc00c);
                    memcpy(answer, &Name, sizeof(unsigned short));
                    length += sizeof(unsigned short);
                    unsigned short TypeSOA = htons(0x0006);
                    memcpy(answer + length, &TypeSOA, sizeof(unsigned short));
                    length += sizeof(unsigned short);


                    unsigned short ClassA = htons(0x0001);
                    memcpy(answer + length, &ClassA, sizeof(unsigned short));
                    length += sizeof(unsigned short);

                    unsigned long timeLive = htonl(0x7b);
                    memcpy(answer + length, &timeLive, sizeof(unsigned long));
                    length += sizeof(unsigned long);
//                    unsigned short ResourceDataLength;
//                    if (type == "IPV4") {
//                        ResourceDataLength = htons(0x0004);
//                    } else if (type=="IPV6"){
//                        ResourceDataLength = htons(0x0010);
//                    } else {
//                        ResourceDataLength = htons(0x0004);
//                    }
//                    memcpy(answer + length, &ResourceDataLength, sizeof(unsigned short));
//                    length += sizeof(unsigned short);
//                    char *Ip = const_cast<char *>(ip.c_str());
//                    auto IP = inet_addr(Ip);
//                    if (type == "IPV4") {
//                        memcpy(answer + length, &IP, sizeof(unsigned long));
//                    } else if (type=="IPV6"){
//                        memcpy(answer + length, &IP, sizeof(IP));
//                    } else {
//                        memcpy(answer + length, &IP, sizeof(unsigned long));
//                    }
//                    length += sizeof(unsigned long);
//                    length +=  rec_len;
//                    memcpy(send_buf +  rec_len, answer, length);

                    int isSend;
                    isSend = sendto(localSoc, send_buf, length, 0, (SOCKADDR*)&receive_in, sizeof(receive_in));
                    if (!isSend) {
                        printf("send failed");
                    }
                }
            } else {
                type=query->type;
                URL = MessageDealer::getHostName(tmp_ptr + 12, tmp_ptr); // 读取域名
                std::string ip = store.getStoredIpByDomain(URL);   //查看是否在本地表中
                EM_IP_TYPE ipType= IP_UNKNOW;
                functions function;
                ipType=function.Check_IP(ip);
                if (ip.empty()) {
                    functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len,debug_mode);
                } else if (ip == "nigeiwoligiaogiao") {
                    break; // ********************************
                } else {
                    if ((function.Get_Type_Name(ipType)!=type)) {
                        functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len,debug_mode);
                    }
                    else
                        functions::sendingBack(rece_buff, ip, receive_in, localSoc, rec_len,type);
                }
            }
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