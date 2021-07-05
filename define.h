//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_DEFINE_H
#define CM_DNS_CPP_DEFINE_H
#define PORT 53
#define MAX_BUFFER_SIZE 1024
#define LOCAL_DNS_ADDR "127.0.0.1"
#define SERVE_DNS_ADDR "10.3.9.45"
//DNS域名解析表结构
typedef struct dns_entry
{
    char * IP;
    char * domain;
} DNS_entry;
#endif //CM_DNS_CPP_DEFINE_H
