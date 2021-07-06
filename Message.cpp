//
// Created by 92798 on 2021/7/6.
//

#include "Message.h"

#include <utility>


DNS_QUERY* Message::getQuery() {
    return this->query;
}

std::vector<DNS_RESPONSE> Message::getResponses() {
    return this->responses;
}

DNS_HEADER* Message::getHeader() {
    return this->header;
}

void Message::setQuery(DNS_QUERY* dnsQuery) {
    this->query=dnsQuery;
}

void Message::setHeader(DNS_HEADER* dnsHeader) {
    this->header=dnsHeader;
}

void Message::setResponses(std::vector<DNS_RESPONSE> dnsResponses) {
    this->responses=std::move(dnsResponses);
}

