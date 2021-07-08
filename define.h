//
// Created by 92798 on 2021/7/5.
//

#ifndef CM_DNS_CPP_DEFINE_H
#define CM_DNS_CPP_DEFINE_H
#define PORT 53
#define EXTERN_PORT 12345
#define MAX_BUFFER_SIZE 1024
#define LOCAL_DNS_ADDR "127.0.0.1"
#define SERVER_DNS_ADDR "10.3.9.44"
#define TABLE_SIZE 200
#define ID_AMOUNT 2000

#include <winsock2.h>
#include <windows.h>
#include <bits/stdc++.h>
#include <unordered_map>
//DNS域名解析表结构

//typedef struct dns_entry
//{
//    char * IP;
//    char * domain;
//} DNS_ENTRY;
typedef struct IDChange//ID转换表结构
{
    unsigned short oldID;            //原有ID
    BOOL done;                        //标记是否完成解析
    SOCKADDR_IN client;                //请求者套接字地址
} IDTransform;

enum EM_IP_TYPE {
    IP_V4 = 0,
    IP_V6,
    IP_UNKNOW,
};

#endif //CM_DNS_CPP_DEFINE_H
