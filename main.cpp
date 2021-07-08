#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <cstring>
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#include "MessageDealer.h"
#include "DNSStore.h"
#include "functions.h"

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
    int debug_mode = getState(mode);
    std::string ip_str(server_ip);
    std::cout<<MessageDealer::isIPValid(ip_str)<<std::endl;
    if(!MessageDealer::isIPValid(ip_str)){
        std::cout<<"Invalid server ip"<<std::endl;
        exit(-1);
    }
    std::cout << "debug_mode:" << mode << std::endl;
    //WSA init
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0) {
        return -1;
    }
    DNSStore store;
    std::string a(file_path);
    store.initLocalTable(a);

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
    server_in.sin_addr.s_addr = inet_addr(server_ip);
    if (bind(localSoc, (LPSOCKADDR) &local_in, sizeof(local_in)) == SOCKET_ERROR) {
        std::cout << "bind error !" << std::endl;
        exit(-1);
    } else {
        if (debug_mode)
            std::cout << "bind local dns relay socket,port:" << PORT << std::endl;
    }
    if (bind(externSoc, (LPSOCKADDR) &extern_in, sizeof(extern_in)) == SOCKET_ERROR) {
        std::cout << "bind error !" << std::endl;
        exit(-1);
    } else {
        if (debug_mode)
            std::cout << "bind local socket to server, port:" << EXTERN_PORT << std::endl;
    }
    struct timeval tv_out; // 设置超时
    tv_out.tv_sec = 0;
    tv_out.tv_usec = 2000000;
    if (setsockopt(externSoc, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_out, sizeof(tv_out)) == -1) {
        std::cout <<  "External Socket setsockopt failed:" << std::endl;
    }
    if (setsockopt(localSoc, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_out, sizeof(tv_out)) == -1) {
        std::cout <<  "Local Socket setsockopt failed:" << std::endl;
    }


    char rece_buff[MAX_BUFFER_SIZE];
    while (1) {
        int len_rece = sizeof(receive_in);
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        int rec_len;
        rec_len = recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *) &receive_in,
                           &len_rece); //收到local
        if (rec_len != -1 && rec_len != 0) {
            std::string type;
            char *tmp_ptr = rece_buff;
            Message local_message=MessageDealer::messageInit(tmp_ptr,false);
            if(debug_mode)
                DetailedLogDealer::receiveLocal(rec_len, receive_in, local_message, server_ip, PORT, tmp_ptr, rec_len);
            else
                SimpleLogDealer::receiveLocal(rec_len,receive_in,local_message);

            DNS_QUERY *query = local_message.getQuery();
            if (query->type != "IPV4" && query->type != "IPV6") {// type not A & AAAA
                if(query->type=="PTR"){
                    functions::sendBackPTR(rece_buff, rec_len, receive_in, localSoc, debug_mode);
                }else
                    functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len, debug_mode);

            } else {
                type = query->type;
                URL = MessageDealer::getHostName(tmp_ptr + 12, tmp_ptr); // 读取域名
                std::string ip;
                if (type=="IPV4") {
                    ip = store.getStoredIpByDomain(URL);   //查看是否在本地表中
                } else {
                    ip = store.getStoredIp6ByDomain(URL);   //查看是否在本地表中
                }
                EM_IP_TYPE ipType = IP_UNKNOW;
                functions function;
                ipType = function.Check_IP(ip);
                if (ip.empty()) {
                    functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len, debug_mode);
                }
                else if (ip == "nigeiwoligiaogiao") {
                    break; // ********************************
                }
                else {
//                    if ((function.Get_Type_Name(ipType)!=type)) {
//                        functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len, debug_mode);
//                    }
//                    else {
                        functions::sendingBack(rece_buff, ip, receive_in, localSoc, rec_len, type, debug_mode);
//                    }
                }
            }
        }
    }

    return 0;
}

int getState(char *state) {
    int len = strlen(state);
    if (memcmp(state, "-d", len) == 0)
        return 0;
    else if (memcmp(state, "-dd", len) == 0)
        return 1;
    else {
        std::cout << "wrong input of debug mode, please input -dd or -d" << std::endl;
        exit(-1);
    }
}