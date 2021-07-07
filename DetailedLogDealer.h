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
class DetailedLogDealer {
public:
     static void receiveLocal(int len, struct sockaddr_in receive_in,Message message);
    static void receiveInternal(Message message);
    static void receiveExternal(Message message);
     static void readLocalAddr(int len, struct sockaddr_in receive_in);
};

#endif //CM_DNS_CPP_DETAILEDLOGDEALER_H
