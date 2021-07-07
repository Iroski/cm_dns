//
// Created by 92798 on 2021/7/7.
//

#include "DetailedLogDealer.h"



void DetailedLogDealer::receiveLocal(int len, struct sockaddr_in receive_in,Message message) {
    std::cout<<"\n\n-----------------------------------NEW QUERY----------------------------------------"<<std::endl;
    readLocalAddr(len,receive_in);
    MessageDealer::printDetailedInfo(std::move(message));
}

void DetailedLogDealer::readLocalAddr(int len, struct sockaddr_in receive_in) {
    long q = receive_in.sin_addr.s_addr;
    char s[40];
    sprintf(s, "%ld", q);
    std::cout<<"Received from local : "<<MessageDealer::charToIpv4(s)<<":"<<receive_in.sin_port<<" ("<<len<<" bytes)"<<std::endl;
}

void DetailedLogDealer::receiveExternal(Message message) {
    std::cout << "----------EXTERN RESPONSE---------" << std::endl;
    MessageDealer::printDetailedInfo(message);
}

void DetailedLogDealer::receiveInternal(Message message) {
    std::cout << "----------INTERNAL RESPONSE---------" << std::endl;
    MessageDealer::printDetailedInfo(message);
}

