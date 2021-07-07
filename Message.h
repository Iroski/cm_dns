//
// Created by 92798 on 2021/7/6.
//

#ifndef CM_DNS_CPP_MESSAGE_H
#define CM_DNS_CPP_MESSAGE_H

#include <winsock2.h>
#include <string>
#include <vector>

#define MESSAGE_HEADER_LENGTH 12
typedef struct dns_header {
    u_short id;
    u_short flags;
    u_short question;
    u_short answer_RR;
    u_short authority_RR;
    u_short additional_RR;
} DNS_HEADER;
typedef struct dns_query {
    std::string name;
    std::string type;
    u_short class_;
    int headerAndQueryLength;
} DNS_QUERY;
typedef struct dns_response {
    std::string name;
    std::string type;
    u_short class_;
    u_long ttl;
    u_short data_length;
    std::string data;
} DNS_RESPONSE;

class Message {
    DNS_HEADER* header;
    DNS_QUERY* query;
    std::vector<DNS_RESPONSE> responses;
public:

    DNS_QUERY* getQuery();

    DNS_HEADER* getHeader();

    std::vector<DNS_RESPONSE> getResponses();

    void setQuery(DNS_QUERY* dnsQuery);

    void setHeader(DNS_HEADER* dnsHeader);

    void setResponses(std::vector<DNS_RESPONSE> dnsResponses);
};


#endif //CM_DNS_CPP_MESSAGE_H
