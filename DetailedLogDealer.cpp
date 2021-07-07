//
// Created by 92798 on 2021/7/7.
//

#include "DetailedLogDealer.h"

#include <utility>


void DetailedLogDealer::receiveLocal(int len, struct sockaddr_in receive_in, Message message, const std::string& server_ip,
                                     int server_port,char* ptr,int ptr_len) {
    std::cout << "\n\n-----------------------------------NEW QUERY----------------------------------------"
              << std::endl;
    readLocalAddr(len, receive_in, std::move(server_ip), server_port);
    MessageDealer::printDetailedInfo(std::move(message));
    for(int i=0;i<ptr_len;++i){
        printf("%02x ",(uint8_t) ptr[i]);
    }
    std::cout<<std::endl;
}

void DetailedLogDealer::readLocalAddr(int len, struct sockaddr_in receive_in, const std::string& server_ip, int server_port) {
    long q = receive_in.sin_addr.s_addr;
    char s[40];
    time_t now = time(0);
    std::string dt = ctime(&now);
    dt = dt.substr(4, 20);
    sprintf(s, "%ld", q);
    std::cout << "Received from local:" << MessageDealer::charToIpv4(s) << ":" << receive_in.sin_port << "  Send to:"
              << server_ip << ":" << server_port << " (" << len
              << " bytes)" << " Time:" << dt << std::endl;

}

void DetailedLogDealer::receiveExternal(Message message,char* ptr,int ptr_len) {
    std::cout << "----------EXTERN RESPONSE---------" << std::endl;
    MessageDealer::printDetailedInfo(std::move(message));
    for(int i=0;i<ptr_len;++i){
        printf("%02x ",(uint8_t) ptr[i]);
    }
    std::cout<<std::endl;
}

void DetailedLogDealer::receiveInternal(const Message &message,char* ptr,int ptr_len) {
    std::cout << "----------INTERNAL RESPONSE---------" << std::endl;
    MessageDealer::printDetailedInfo(message);
    if (MessageDealer::isIntercept(message)) {
        std::cout << "++++++++THIS QUERY IS BE INTERCEPTED++++++++" << std::endl;
    }
    for(int i=0;i<ptr_len;++i){
        printf("%02x ",(uint8_t) ptr[i]);
    }
    std::cout<<std::endl;
}

