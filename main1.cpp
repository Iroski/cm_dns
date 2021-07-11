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
    char *file_path = "dnsrelay.txt";

    if (argc >= 1)
        mode = argv[1]; //0:-d  1:-dd
    if (argc >= 2)
        server_ip = argv[2];
    if (argc >= 3)
        file_path = argv[3];

    int debug_mode = getState(mode);

    std::string ip_str(server_ip);
    std::cout << MessageDealer::isIPValid(ip_str) << std::endl;
    if (!MessageDealer::isIPValid(ip_str)) {
        std::cout << "Invalid server ip" << std::endl;
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

    fd_set fdSet;

    struct timeval time_out; // 设置超时
    time_out.tv_sec = 60;
    time_out.tv_usec = 0;
    if (setsockopt(externSoc, SOL_SOCKET, SO_RCVTIMEO, (char *) &time_out, sizeof(time_out)) == -1) {
        std::cout << "External Socket set time out failed" << std::endl;
    }
    if (setsockopt(localSoc, SOL_SOCKET, SO_RCVTIMEO, (char *) &time_out, sizeof(time_out)) == -1) {
        std::cout << "Inernal Socket set time out failed" << std::endl;
    }

    int selected_net;
    char rece_buff[MAX_BUFFER_SIZE];
    while (1) {
        FD_ZERO(&fdSet);
        FD_SET(localSoc, &fdSet);
        FD_SET(externSoc, &fdSet);

        selected_net = select(0, &fdSet, nullptr, nullptr, &time_out);
        int len_rece = sizeof(receive_in);
        int rec_len;
        memset(rece_buff, 0, MAX_BUFFER_SIZE); //将接收缓存先置为全0

        if (selected_net == SOCKET_ERROR) {
            std::cout << "Internal error!";
            closesocket(localSoc);
            closesocket(externSoc);
            return -1;
        }  else {
            if (FD_ISSET(localSoc, &fdSet)) {
                rec_len = recvfrom(localSoc, rece_buff, sizeof(rece_buff), 0, (struct sockaddr *) &receive_in,
                                   &len_rece); //收到local
                if (rec_len != -1 && rec_len != 0) {
                    std::string type;
                    char *tmp_ptr = rece_buff;

                    Message local_message = MessageDealer::messageInit(tmp_ptr, false);
                    if (debug_mode)
                        DetailedLogDealer::receiveLocal(rec_len, receive_in, local_message, server_ip, PORT, tmp_ptr,
                                                        rec_len);
                    else
                        SimpleLogDealer::receiveLocal(rec_len, receive_in, local_message);


                    DNS_QUERY *query = local_message.getQuery();
                    if (query->type != "IPV4" && query->type != "IPV6") {// type not A & AAAA
                        if (query->type == "PTR") {
                            functions::sendBackPTR(rece_buff, rec_len, receive_in, localSoc, debug_mode);
                        } else
                            functions::forwardSelectQuery(rece_buff, receive_in, server_in, externSoc, localSoc,
                                                          rec_len, debug_mode);

                    } else {
                        type = query->type;
                        URL = MessageDealer::getHostName(tmp_ptr + 12, tmp_ptr); // 读取域名

                        std::string ip4;
                        std::string ip6;
                        ip4 = store.getStoredIpByDomain(URL);   //查看是否在本地表中
                        ip6 = store.getStoredIp6ByDomain(URL);   //查看是否在本地表中
                        EM_IP_TYPE ipType = IP_UNKNOW;
                        functions function;
                        ipType = function.Check_IP(ip4);
                        if (type=="IPV4") {
                            if ((ip4.empty()&&!ip6.empty()&&ip6!="0:0:0:0:0:0:0:0")||(ip4.empty()&&ip6.empty())) {
                                functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len, debug_mode);
                            }
                            else if (ip4 == "111111111111111111") {
                                break; // ********************************
                            }
                            else {
                                if (ip4 == "0.0.0.0"||ip6=="0:0:0:0:0:0:0:0") {
                                    functions::sendingBack(rece_buff, "0.0.0.0", receive_in, localSoc, rec_len, type, debug_mode);
                                }
                                else {
                                    functions::sendingBack(rece_buff, ip4, receive_in, localSoc, rec_len, type, debug_mode);
                                }
                            }
                        }
                        else {
                            if ((ip6.empty()&&!ip4.empty()&&ip4!="0.0.0.0")||(ip4.empty()&&ip6.empty())) {
                                functions::forwardQuery(rece_buff, receive_in, server_in, externSoc, localSoc, rec_len, debug_mode);
                            }
                            else if (ip4 == "111111111111111111") {
                                break; // ********************************
                            }
                            else {
                                if (ip4 == "0.0.0.0"||ip6=="0:0:0:0:0:0:0:0") {
                                    functions::sendingBack(rece_buff, "0:0:0:0:0:0:0:0", receive_in, localSoc, rec_len, type, debug_mode);
                                }
                                else {
                                    functions::sendingBack(rece_buff, ip6, receive_in, localSoc, rec_len, type, debug_mode);
                                }
                            }
                        }
                    }
                }
            }
            if (FD_ISSET(externSoc, &fdSet)) {
                int recv_len = recvfrom(externSoc, rece_buff, MAX_BUFFER_SIZE, 0, nullptr, nullptr); //接受从远端发回的信息
                if (recv_len == -1) {
                    std::cout << "Receive Timeout" << std::endl;
                }

                if (recv_len != -1 && recv_len != 0) {
                    char *tmp_ptr = rece_buff;
                    Message message = MessageDealer::messageInit(tmp_ptr, true);

                    if (debug_mode) {
                        DetailedLogDealer::receiveExternal(message, tmp_ptr, recv_len);
                    } else {
                        SimpleLogDealer::receiveExternal(message);
                    }
                }
                unsigned short *recv_ID = new unsigned short();
                //ID转换
                memcpy(recv_ID, rece_buff, sizeof(unsigned short)); //报文前两字节为ID
                unsigned short oID = htons(IDTransTable[ntohs(*recv_ID)].oldID);  // 得到old id 发回给receive
                memcpy(rece_buff, &oID, sizeof(unsigned short));
                sockaddr_in client = IDTransTable[ntohs(*recv_ID)].client;
                int send_len = sendto(localSoc, rece_buff, recv_len, 0, (SOCKADDR *) &client, sizeof(client));
                if (recv_len == -1) {
                    std::cout << "Send Timeout" << std::endl;
                }
                if (send_len != -1 && send_len != 0) {

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