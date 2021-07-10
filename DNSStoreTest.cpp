//
// Created by 1 on 2021/7/6.
//

#include "DNSStoreTest.h"
#include <iostream>
#include <stdio.h>
#include "DNSStore.h"
#include "define.h"

int main() {
    DNSStore store = DNSStore();
    printf("hello");
    store.initLocalTable(PATH);
    std::cout << store.getStoredIpByDomain("www.666.com") << " "
              << store.getStoredIpByDomain("www.bupt.com.cn") << " "
              << store.getStoredIpByDomain("www.baidu.com") << std::endl;
    //std::cout<<DNSTable<<std::endl;
    for (auto &iter : store_map) {
        std::cout << iter.first << " " << iter.second << std::endl;
    }
    for (auto &iter : store_map6) {
        std::cout << iter.first << " " << iter.second << std::endl;
    }
    return 0;
}
