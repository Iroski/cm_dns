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
    char* str="24098c006c21103d000000ffb00239ab";
   std::cout<<MessageDealer::charToIpv6(str)<<std::endl;

    exit(1);
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
    struct sockaddr_in serve_in, local_in,receive_in;
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
        printf("bind socket success for test\n");



    char rece_buff[MAX_BUFFER_SIZE];
    while(true){
        int len_rece = sizeof(receive_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        int rec_len;
        rec_len= recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *)&receive_in, &len_rece);

        if(rec_len != -1 && rec_len != 0){
            unsigned short iSend;
            iSend = sendto(serveSoc, rece_buff, rec_len, 0, (SOCKADDR*)&serve_in, sizeof(serve_in));
            clock_t start, stop; //定时
            double duration = 0;
            start = clock();
            rec_len = recvfrom(serveSoc, rece_buff, sizeof(rece_buff), 0, (SOCKADDR*)&receive_in, &len_rece);
            while ((rec_len == 0) || (rec_len == SOCKET_ERROR))
            {
                rec_len = recvfrom(serveSoc, rece_buff, sizeof(rece_buff), 0, (SOCKADDR*)&receive_in, &len_rece);
                stop = clock();
                duration = (double)(stop - start) / CLK_TCK;
                if (duration > 5)
                {
                    printf("Long Time No Response From Server.\n");
                    break;
                }
            }
            if(rec_len != -1 && rec_len != 0){
                char* tmp_ptr=rece_buff;
                DNS_HEADER* header=MessageDealer::getDNSHeader(tmp_ptr);
                DNS_QUERY * query=MessageDealer::getDNSQuery(tmp_ptr);
                MessageDealer::printQueryAll(query);
                MessageDealer::printHeaderAll(header);
            }

        }


    }

    return 0;
}