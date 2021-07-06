//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_DEFINE_H
#define CM_DNS_CPP_DEFINE_H
#define PORT 53
#define EXTERN_PORT 8000
#define MAX_BUFFER_SIZE 1024
#define LOCAL_DNS_ADDR "127.0.0.1"
#define SERVER_DNS_ADDR "10.3.9.44"
#define TABLE_SIZE 200
#define ID_AMOUNT 2000
#define PATH "dnsrelay.txt"

#include <winsock2.h>
#include <bits/stdc++.h>
#include <unordered_map>
//DNS域名解析表结构

//typedef struct dns_entry
//{
//    char * IP;
//    char * domain;
//} DNS_ENTRY;
typedef struct dns_header{
    u_short ID;
    u_short Flags;
    u_short Question;
    u_short Answer_RR;
    u_short Authority_RR;
    u_short Additional_RR;
}DNS_HEADER;
typedef struct dns_query{
    std::string name;
    u_short type;
    u_short class_;
    int headerAndQueryLength;
}DNS_QUERY;
typedef struct IDChange//ID转换表结构
{
    unsigned short oldID;			//原有ID
    BOOL done;						//标记是否完成解析
    SOCKADDR_IN client;				//请求者套接字地址
}IDTransform;
#endif //CM_DNS_CPP_DEFINE_H
