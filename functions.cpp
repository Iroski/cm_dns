//
// Created by 1 on 2021/7/6.
//

#include "functions.h"
#include "MessageDealer.h"
#include "define.h"
#include "DNSStore.h"
extern std::string URL;

static void printDNSInformation(unsigned short ID, int find, std::string ip) {
    printf("%d",ID);
    if (find == 0) {
        printf("RELAY");
        printf("    ");
        std::cout<< URL <<std::endl;
        printf("    ");
        std::cout<< ip <<std::endl;
        printf("\n");
    }
    else {
        if (ip=="0.0.0.0") {
            printf("SHIELD");
            printf("   ");
            std::cout<< URL <<std::endl;
            printf("    ");
            std::cout<< ip <<std::endl;
            printf("\n");
        }
        else {
            printf("LOCAL");
            printf("    ");
            std::cout<< URL <<std::endl;
            printf("    ");
            std::cout<< ip <<std::endl;
            printf("\n");
        }

    }
}
