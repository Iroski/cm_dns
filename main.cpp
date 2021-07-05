#include <bits/stdc++.h>
#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
using namespace std;
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

        //接收DNS请求
        //函数：int recvfrom(int s, void* buf, int len, unsigned int flags, struct sockaddr* from, int* fromlen);
        //函数说明：recv()用来接收远程主机经指定的socket 传来的数据, 并把数据存到由参数buf 指向的内存空间, 参数len 为可接收数据的最大长度.
        //参数flags 一般设0, 其他数值定义请参考recv().参数from 用来指定欲传送的网络地址, 结构sockaddr 请参考bind().参数fromlen 为sockaddr 的结构长度.
        int iRecv;
        iRecv= recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *)&reveice_in, &len_rece);

        if(iRecv!=-1&&iRecv!=0){
            char* tmp_ptr=rece_buff;
            int t_id= ntohs(*(u_short *)tmp_ptr);
            tmp_ptr+=sizeof(t_id);
            int type=ntohs(*(u_short *)tmp_ptr);
            tmp_ptr+=sizeof(u_short *)*4;//忽略掉中间RR
//            long domain= ntohs(*(u_short *) tmp_ptr);
            int len=std::strlen(tmp_ptr);
            char* domain=(char *)
            memcpy(domain,tmp_ptr,sizeof(byte *)*15);
            cout<<t_id<<" "<<type<<" "<<domain<<endl;
        }


    }

    return 0;
}