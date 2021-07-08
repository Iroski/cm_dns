//
// Created by 92798 on 2021/7/5.
//
#include <stdio.h>
#include <iostream>
#include "DNSStore.h"
#include <fstream>
#include <string>
#include <vector>
#include "functions.h"

int DNSStoreNum;
int DNSStoreNum6;
std::string DNSStore::getStoredIpByDomain(const std::string& domain) {
    if(store_map.count(domain) != 1) {
        return "";
    }
    return store_map[domain];
}

void DNSStore::initLocalTable(const std::string Path) {
    functions function;
    int i=0;
    std::ifstream myfile;
    std::cout << "Load from:"<<Path << std::endl;
    myfile.open(Path, std::ios::in);
    std::string Temp[TABLE_SIZE];
    std::string temp;
    if (!myfile.is_open()) {
        printf("Open file failed");
        exit(-1);
    }
    while (getline(myfile,temp)) {
        if (temp=="") {
            break;
        }
        Temp[i]=temp;
        i++;

        DNSStoreNum++;
        if ((DNSStoreNum+DNSStoreNum6)>TABLE_SIZE) {
            printf("The DNS Store is full!");
            break;
        }
    }
    for (int j=0;j<(DNSStoreNum+DNSStoreNum6);j++) {
        std::vector<std::string> res;
        std::string result;
        std::stringstream input(Temp[j]);
        while (input >> result)
            res.push_back(result);
        if (function.Get_Type_Name(function.Check_IP(res[0]))=="IPV4") {
            store_map[res[1]] = res[0];
        }
        else {
            store_map6[res[1]] = res[0];
            --DNSStoreNum;
            ++DNSStoreNum6;
        }
    }
//    for (auto &iter : store_map) {
//        std::cout << iter.first << " " << iter.second << std::endl;
//    }
//    for (auto &iter : store_map6) {
//        std::cout << iter.first << " " << iter.second << std::endl;
//    }
    printf("Load records success.\n");
}

std::string DNSStore::getStoredIp6ByDomain(const std::string& domain) {
    if(store_map6.count(domain)!=1) {
        return "";
    }
    return store_map6[domain];
}

