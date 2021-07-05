//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_DNSSTORE_H
#define CM_DNS_CPP_DNSSTORE_H
#include <string>

class DNSStore {


public:
    void initLocalTable(std::string Path);
    int checkDomainExist(char* domain);
    std::string getStoredIpByDomain(char* domain);
};


#endif //CM_DNS_CPP_DNSSTORE_H
