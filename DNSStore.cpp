//
// Created by 92798 on 2021/7/5.
//
#include <iostream>
#include "DNSStore.h"
#include <fstream>
#include <string>
#include <vector>
int DNSStoreNum;

std::string DNSStore::getStoredIpByDomain(std::string domain) {
    if(store_map.count(domain) == 0)
        return "";
    return store_map[domain];
}

void DNSStore::initLocalTable(const std::string Path) {
    int i=0;
    std::ifstream myfile;
    std::cout << Path << std::endl;
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
        if (DNSStoreNum>TABLE_SIZE) {
            printf("The DNS Store is full!");
            break;
        }
    }
    printf("%d",DNSStoreNum);
    for (int j=0;j<DNSStoreNum;j++) {
        std::vector<std::string> res;
        std::string result;
        std::stringstream input(Temp[j]);
        while (input >> result)
            res.push_back(result);
        store_map[res[0]] = res[1];
    }
    for (auto &iter : store_map) {
        std::cout << iter.first << " " << iter.second << std::endl;
    }
    printf("Load records success.\n");
}

