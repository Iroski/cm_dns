//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_DNSSTORE_H
#define CM_DNS_CPP_DNSSTORE_H
#include "define.h"
#include <string>
#include <unordered_map>
extern int DNSStoreNum;

class DNSStore {
private:
    std::unordered_map<std::string, std::string> store_map;

public:

    void initLocalTable(const std::string Path);
//    int checkDomainExist(std::string domain);
    std::string getStoredIpByDomain(std::string domain);
};


#endif //CM_DNS_CPP_DNSSTORE_H
