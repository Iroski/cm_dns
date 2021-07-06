//
// Created by 92798 on 2021/7/7.
//

#ifndef CM_DNS_CPP_DETAILEDLOGDEALER_H
#define CM_DNS_CPP_DETAILEDLOGDEALER_H
#include<string>
#include<iostream>
#include <WinSock2.h>
#include <windows.h>
#include "MessageDealer.h"
class DetailedLogDealer {
public:
     static void receiveLocalInit();

     static void readLocalAddr(int len, struct sockaddr_in receive_in);
};

#endif //CM_DNS_CPP_DETAILEDLOGDEALER_H
