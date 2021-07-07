//
// Created by 14766 on 2021/7/7.
//

#include "SimpleLogDealer.h"

void SimpleLogDealer::receiveLocal(int len, struct sockaddr_in receive_in, Message message) {
    std::cout<<"\n\n-----------------------------------NEW QUERY--------------------------------------"<<std::endl;
    std::cout<<"Received from local"<<std::endl;
    MessageDealer::printQuerySimple(message.getQuery());
}

void SimpleLogDealer::receiveInternal(Message message) {
    std::cout << "----------INTERNAL RESPONSE---------" << std::endl;
    MessageDealer::printResponsesSimple(message.getResponses());
}

void SimpleLogDealer::receiveExternal(Message message) {
    std::cout << "----------EXTERN RESPONSE---------" << std::endl;
    MessageDealer::printResponsesSimple(message.getResponses());
}


