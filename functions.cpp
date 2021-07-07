//
// Created by 1 on 2021/7/6.
//

#include "define.h"
#include "functions.h"
#include "MessageDealer.h"
#include "DNSStore.h"
extern std::string URL;
extern IDTransform IDTransTable[ID_AMOUNT];

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

void functions::forwardQuery(char *recvBuf, sockaddr_in receive_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len, int debugMode){
    unsigned short* recv_ID;
    unsigned short send_ID;
    recv_ID = (unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(recv_ID, recvBuf, sizeof(unsigned short));    // 收到报文的ID（前2字节）
    send_ID = htons(MessageDealer::getNewID(ntohs(*recv_ID), receive_in, FALSE));
    //PrintInfo(ntohs(send_ID), not_find);
    memcpy(recvBuf, &send_ID, sizeof(unsigned short));
    int send_len = sendto(externSoc, recvBuf, len, 0, (struct sockaddr *)&server_in, sizeof(server_in));
    free(recv_ID);

    DNS_HEADER *header = MessageDealer::getDNSHeader(recvBuf);
    DNS_QUERY *query = MessageDealer::getDNSQuery(recvBuf);
    //std::cout << send_len << std::endl;
    //std::cout << "send end" << std::endl;

    clock_t start, stop; //定时
    double duration = 0;
    start = clock();
    int size = sizeof(server_in);
    int recv_len = recvfrom(externSoc, recvBuf, MAX_BUFFER_SIZE, 0, nullptr, nullptr); //接受从远端发回的信息

    if (recv_len != -1 && recv_len != 0) {
        char *tmp_ptr = recvBuf;
        Message message = MessageDealer::messageInit(tmp_ptr, true);
        DetailedLogDealer::externalInit();
        MessageDealer::printDetailedInfo(message);
//        if(message.getQuery()->type=="IPV6"){
//            std::cout<<"Start IPV6 process"<<std::endl;
//        }
//        DNS_HEADER *header = MessageDealer::getDNSHeader(tmp_ptr);
//        DNS_QUERY *query = MessageDealer::getDNSQuery(tmp_ptr);
//        std::cout << recv_len << std::endl;
//        std::cout << "receive end" << std::endl;
    }

    //ID转换
    memcpy(recv_ID, recvBuf, sizeof(unsigned short)); //报文前两字节为ID
    unsigned short oID = htons(IDTransTable[ntohs(*recv_ID)].oldID);  // 得到old id 发回给receive
    memcpy(recvBuf, &oID, sizeof(unsigned short));
    sockaddr_in client = IDTransTable[ntohs(*recv_ID)].client;
    send_len = sendto(localSoc, recvBuf, recv_len, 0, (SOCKADDR*)&client, sizeof(client));
    if (send_len != -1 && send_len != 0) {
        char *tmp_ptr = recvBuf;
//       DNS_HEADER *header = MessageDealer::getDNSHeader(tmp_ptr);
//        DNS_QUERY *query = MessageDealer::getDNSQuery(tmp_ptr);
        //std::cout << send_len << std::endl;

//        std::cout << "send to local end" << std::endl;
    }
//    free(recv_ID); //释放动态分配的内存
}

void functions::sendingBack(char *rece_buff, std::string ip, sockaddr_in receive_in, SOCKET localSoc, int rec_len) {
    char send_buf[MAX_BUFFER_SIZE];
    unsigned short *pID=(unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(pID, rece_buff, sizeof(unsigned short));
    memcpy(send_buf,rece_buff,rec_len);
    unsigned short nID=htons(MessageDealer::getNewID(ntohs(*pID), receive_in, FALSE));
    unsigned short AFlag=htons(0x8180);
    memcpy(&send_buf[2], &AFlag, sizeof(unsigned short));
    functions::printDNSInformation(nID,1,ip);
    if (ip=="0.0.0.0") {
        AFlag=htons(0x0000);
        printf("**********  No such name!  **********\n");
    }
    else {
        printf("**************  Have this name!  ****************\n");
        AFlag=htons(0x0001);
    }
    memcpy(&send_buf[6], &AFlag, sizeof(unsigned short));

    int length=0;
    char answer[16];
    unsigned short Name = htons(0xc00c);
    memcpy(answer, &Name, sizeof(unsigned short));
    length += sizeof(unsigned short);
    unsigned short TypeA = htons(0x0001);
    memcpy(answer + length, &TypeA, sizeof(unsigned short));
    length += sizeof(unsigned short);

    unsigned short ClassA = htons(0x0001);
    memcpy(answer + length, &ClassA, sizeof(unsigned short));
    length += sizeof(unsigned short);

    unsigned long timeLive = htonl(0x7b);
    memcpy(answer + length, &timeLive, sizeof(unsigned long));
    length += sizeof(unsigned long);

    unsigned short ResourceDataLength = htons(0x0004);
    memcpy(answer + length, &ResourceDataLength, sizeof(unsigned short));
    length += sizeof(unsigned short);
    char* Ip= const_cast<char *>(ip.c_str());
    auto IP = (unsigned long)inet_addr(Ip);
    memcpy(answer + length, &IP, sizeof(unsigned long));
    length += sizeof(unsigned long);
    length +=  rec_len;
    memcpy(send_buf +  rec_len, answer, length);

    int isSend;
    isSend = sendto(localSoc, send_buf, length, 0, (SOCKADDR*)&receive_in, sizeof(receive_in));
    if (!isSend) {
        printf("send failed");
    }
}
