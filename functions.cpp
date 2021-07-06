//
// Created by 1 on 2021/7/6.
//

#include "functions.h"
#include "MessageDealer.h"
#include "define.h"
#include "DNSStore.h"
extern std::string URL;

void functions::printDNSInformation(unsigned short ID, int find, std::string ip) {
    printf("%d",ID);
    if (find == 0) {
        printf("    RELAY");
        printf("    ");
        std::cout<< URL<<"    "<<ip <<std::endl;
    }
    else {
        if (ip=="0.0.0.0") {
            printf("    SHIELD");
            printf("   ");
            std::cout<< URL<<"    "<<ip <<std::endl;;
        }
        else {
            printf("    LOCAL");
            printf("    ");
            std::cout<< URL<<"    "<<ip <<std::endl;
        }

    }
}
