//
// Created by 92798 on 2021/7/7.
//

#ifndef CM_DNS_CPP_DETAILEDLOGDEALER_H
#define CM_DNS_CPP_DETAILEDLOGDEALER_H
#include<string>
#include<iostream>
#include <WinSock2.h>
#include <windows.h>
#include <utility>
#include "MessageDealer.h"
#include "Message.h"
#include <ctime>
class DetailedLogDealer {
public:
    static void receiveLocal(int len, struct sockaddr_in receive_in,Message message,const std::string& server_ip,int server_port,char* ptr,int ptr_len);
    static void receiveInternal(const Message& message,char* ptr,int ptr_len);
    static void receiveExternal(Message message,char* ptr,int ptr_len);
    static void readLocalAddr(int len, struct sockaddr_in receive_in,const std::string& server_ip, int server_port);
};

#endif //CM_DNS_CPP_DETAILEDLOGDEALER_H
