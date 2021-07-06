//
// Created by 1 on 2021/7/6.
//

#include "functions.h"
#include "MessageDealer.h"
#include "define.h"
#include "DNSStore.h"
static void printDNSInformation(unsigned short ID, int find) {
    if (find == NOT_FOUND) {
        //中继功能
        printf("RELAY");
        printf("    ");
        //打印域名
        //printf("%s",Url);
        printf("    ");
        //打印IP
        printf("\n");
    }

        //在表中找到DNS请求中的域名

    }
}
