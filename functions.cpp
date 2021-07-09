//
// Created by 1 on 2021/7/6.
//


#include "functions.h"
#include "MessageDealer.h"
#include "DNSStore.h"
#include "SimpleLogDealer.h"

extern std::string URL;
extern IDTransform IDTransTable[ID_AMOUNT];



using namespace std;

void functions::forwardQuery(char *recvBuf, sockaddr_in receive_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len, int debugMode){
    unsigned short* recv_ID;
    unsigned short send_ID;
    recv_ID = (unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(recv_ID, recvBuf, sizeof(unsigned short));    // 收到报文的ID（前2字节）
    send_ID = htons(MessageDealer::getNewID(ntohs(*recv_ID), receive_in, FALSE));
    memcpy(recvBuf, &send_ID, sizeof(unsigned short));
    int send_len = sendto(externSoc, recvBuf, len, 0, (struct sockaddr *)&server_in, sizeof(server_in));
    free(recv_ID);

    int recv_len = recvfrom(externSoc, recvBuf, MAX_BUFFER_SIZE, 0, nullptr, nullptr); //接受从远端发回的信息
    if(recv_len == -1){
        std::cout << "Receive Timeout" << std::endl;
    }

    if (recv_len != -1 && recv_len != 0) {
        char *tmp_ptr = recvBuf;
        Message message = MessageDealer::messageInit(tmp_ptr, true);

        if(debugMode){
            DetailedLogDealer::receiveExternal(message,tmp_ptr,recv_len);
        }
       else {
            SimpleLogDealer::receiveExternal(message);
        }
    }

    //ID转换
    memcpy(recv_ID, recvBuf, sizeof(unsigned short)); //报文前两字节为ID
    unsigned short oID = htons(IDTransTable[ntohs(*recv_ID)].oldID);  // 得到old id 发回给receive
    memcpy(recvBuf, &oID, sizeof(unsigned short));
    sockaddr_in client = IDTransTable[ntohs(*recv_ID)].client;
    send_len = sendto(localSoc, recvBuf, recv_len, 0, (SOCKADDR*)&client, sizeof(client));
    if(recv_len == -1){
        std::cout << "Send Timeout" << std::endl;
    }
    if (send_len != -1 && send_len != 0) {

    }
}

void functions::forwardSelectQuery(char *recvBuf, sockaddr_in receive_in, sockaddr_in server_in, SOCKET &externSoc, SOCKET localSoc, int len, int debugMode){
    unsigned short* recv_ID;
    unsigned short send_ID;
    recv_ID = (unsigned short*)malloc(sizeof(unsigned short*));
    memcpy(recv_ID, recvBuf, sizeof(unsigned short));    // 收到报文的ID（前2字节）
    send_ID = htons(MessageDealer::getNewID(ntohs(*recv_ID), receive_in, FALSE));
    memcpy(recvBuf, &send_ID, sizeof(unsigned short));
    int send_len = sendto(externSoc, recvBuf, len, 0, (struct sockaddr *)&server_in, sizeof(server_in));
    free(recv_ID);

}

void functions::sendingBack(char *rece_buff, std::string ip, sockaddr_in receive_in, SOCKET localSoc, int rec_len,std::string type, int debug_mode) {
    //char send_buf[MAX_BUFFER_SIZE];
    char *tmp_ptr = rece_buff;
    char send_buf[MAX_BUFFER_SIZE];

    Message message=MessageDealer::messageInit(tmp_ptr,true);
    MessageDealer::getDNSHeader(rece_buff);
    DNS_QUERY *query=message.getQuery();

    int len=query->headerAndQueryLength;
    memcpy(send_buf, rece_buff, len);
    unsigned short *pID = (unsigned short *) malloc(sizeof(unsigned short *));
    memcpy(pID, rece_buff, sizeof(unsigned short));
    memcpy(send_buf, rece_buff, rec_len);
    unsigned short nID = htons(MessageDealer::getNewID(ntohs(*pID), receive_in, FALSE));;//***********************
    //functions::printDNSInformation(nID, 1, ip);
    unsigned short AFlag;
    if (ip == "0.0.0.0"||ip == "0:0:0:0:0:0:0:0") {
        unsigned short AFlag = htons(0x8183); //1000 0001 1000 0011
        memcpy(&send_buf[2], &AFlag, sizeof(unsigned short));
    } else {
        unsigned short AFlag = htons(0x8180); //1000 0001 1000 0000
        memcpy(&send_buf[2], &AFlag, sizeof(unsigned short));
    }

    unsigned short ansCount;
    if (ip == "0.0.0.0"||ip == "0:0:0:0:0:0:0:0") {
        ansCount = htons(0x0000);
        //printf("**********  No such name!  **********\n");
    } else {
        //printf("**************  Have this name!  ****************\n");
        ansCount = htons(0x0001);
    }
    memcpy(&send_buf[6], &ansCount, sizeof(unsigned short));

    int length = len;
    unsigned short Name = htons(0xc00c);
    memcpy(send_buf+length, &Name, sizeof(unsigned short));
    length += sizeof(unsigned short);
    if (type == "IPV4") {
        unsigned short TypeA = htons(0x0001);
        memcpy(send_buf+length, &TypeA, sizeof(unsigned short));
    } else if (type=="IPV6"){
        unsigned short TypeAAAA = htons(0x001C);
        memcpy(send_buf+length, &TypeAAAA, sizeof(unsigned short));
    }
    else {
        unsigned short TypeA = htons(0x0001);
        memcpy(send_buf+length, &TypeA, sizeof(unsigned short));
    }
    length += sizeof(unsigned short);


    unsigned short ClassA = htons(0x0001);
    memcpy(send_buf+length, &ClassA, sizeof(unsigned short));
    length += sizeof(unsigned short);

    unsigned long timeLive = htonl(0x7b);
    memcpy(send_buf+length, &timeLive, sizeof(unsigned long));
    length += sizeof(unsigned long);
    unsigned short ResourceDataLength;
    if (type == "IPV4") {
        ResourceDataLength = htons(0x0004);
    } else if (type=="IPV6"){
        ResourceDataLength = htons(0x0010);
    } else {
        ResourceDataLength = htons(0x0004);
    }
    memcpy(send_buf+length, &ResourceDataLength, sizeof(unsigned short));
    length += sizeof(unsigned short);
    char *Ip = const_cast<char *>(ip.c_str());

    if (type == "IPV4") {
        auto IP = inet_addr(Ip);
        memcpy(send_buf+length, &IP, sizeof(unsigned long)); //ffff:ffff:0:10:0:7b:0:1
        length += sizeof(unsigned long);
    } else {
        std::string Ip_str(Ip);
        char* IP = MessageDealer::ipv6ToChar(Ip_str);
        memcpy(send_buf+length, IP, sizeof(unsigned long)*4);
        length += sizeof(unsigned long)*4;
    }

    int isSend;
    isSend = sendto(localSoc, send_buf, length, 0, (SOCKADDR*)&receive_in, sizeof(receive_in));
    if (!isSend) {
        printf("send failed");
    }else{
        Message message = MessageDealer::messageInit(send_buf,true);
        if(debug_mode){
            DetailedLogDealer::receiveInternal(message,send_buf,isSend);
        }
        else{
            SimpleLogDealer::receiveInternal(message);
        }
    }
}

void functions::sendBackPTR(char *rece_buff, int rec_len,sockaddr_in receive_in, SOCKET localSoc, int debug_mode) {
    char *tmp_ptr = rece_buff;
    char send_buf[MAX_BUFFER_SIZE];

    Message message=MessageDealer::messageInit(tmp_ptr,true);
    MessageDealer::getDNSHeader(rece_buff);
    DNS_QUERY *query=message.getQuery();
    int len=query->headerAndQueryLength;
    memcpy(send_buf, rece_buff, len);

    unsigned short AFlag = htons(0x8180); //1000 0001 1000 0000
    memcpy(&send_buf[2], &AFlag, sizeof(unsigned short));
    int length=len;
    unsigned short Name = htons(0xc00c);
    memcpy(send_buf+length, &Name, sizeof(unsigned short));
    length += sizeof(unsigned short);

    unsigned short TypeSOA = htons(0x0006);
    memcpy(send_buf+length, &TypeSOA, sizeof(unsigned short));
    length += sizeof(unsigned short);


    unsigned short ClassA = htons(0x0001);
    memcpy(send_buf+length, &ClassA, sizeof(unsigned short));
    length += sizeof(unsigned short);

    unsigned long timeLive = htonl(0x7b);
    memcpy(send_buf+length, &timeLive, sizeof(unsigned long));
    length += sizeof(unsigned long);

    unsigned short ResourceDataLength;
    ResourceDataLength = htons(0x0000);
    memcpy(send_buf+length, &ResourceDataLength, sizeof(unsigned short));
    length += sizeof(unsigned short);

//    length +=  rec_len;
//    memcpy(send_buf +  rec_len, answer, length);

    int isSend;
    isSend = sendto(localSoc, send_buf, length, 0, (SOCKADDR*)&receive_in, sizeof(receive_in));
    if (!isSend) {
        printf("send failed");
    }
    else{
        Message message = MessageDealer::messageInit(send_buf,true);
        if(debug_mode)
            DetailedLogDealer::receiveInternal(message, tmp_ptr, rec_len);
        else
            SimpleLogDealer::receiveInternal(message);
    }
}

void functions::str_split(const string &str, const string &sign, vector<std::string> &results) {
    string::size_type pos;
    size_t size = str.size();
    for (size_t i = 0; i < size; ++i)
    {
        pos = str.find(sign, i);
        if (pos == str.npos)
        {
            string s = str.substr(i, size);
            results.push_back(s);
            break;
        }
        if (pos < size)
        {
            string s = str.substr(i, pos - i);
            results.push_back(s);
            i = pos;
        }
    }
}

EM_IP_TYPE functions::Check_IP_V6(std::vector<std::string> vecIpSection) {
    //printf("[Check_IP_V6]size=%d.\n", vecIpSection.size());
    if(vecIpSection.size() != 8)
    {
        return IP_UNKNOW;
    }

    //判断每个IP段的合法性
    for(int i = 0; i < vecIpSection.size(); i++)
    {
        if(vecIpSection[i].c_str() == "")
        {
            continue;
        }

        char* pStop = NULL;
        long int nSection = (long int)strtol(vecIpSection[i].c_str(), &pStop, 16);

        if((int)(pStop - vecIpSection[i].c_str()) != vecIpSection[i].length())
        {
            printf("[Check_IP_V4]error Section[%s].\n", vecIpSection[i].c_str());
            return IP_UNKNOW;
        }

        //printf("[Check_IP_V4]nSection=%d.\n", nSection);
        if(nSection < 0x0000 || nSection > 0xffff)
        {
            return IP_UNKNOW;
        }
    }

    return IP_V6;
}

EM_IP_TYPE functions::Check_IP(std::string strIP) {
    vector<string> vecIpSection;

    str_split(strIP, ".", vecIpSection);
    //printf("[Check_IP]v4 vecIpSection size=%d.\n", vecIpSection.size());
    if(vecIpSection.size() > 1)
    {
        return Check_IP_V4(vecIpSection);
    }

    vecIpSection.clear();
    str_split(strIP, ":", vecIpSection);
    //printf("[Check_IP]v6 vecIpSection size=%d.\n", vecIpSection.size());
    if(vecIpSection.size() > 1)
    {
        return Check_IP_V6(vecIpSection);
    }

    return IP_UNKNOW;
}

std::string functions::Get_Type_Name(EM_IP_TYPE emType) {
    if(IP_V4 == emType)
    {
        return "IPV4";
    }
    else if(IP_V6 == emType)
    {
        return "IPV6";
    }
    else
    {
        return "IP Unknow";
    }
}

EM_IP_TYPE functions::Check_IP_V4(std::vector<std::string> vecIpSection) {
    //printf("[Check_IP_V4]size=%d.\n", vecIpSection.size());
    if(vecIpSection.size() != 4)
    {
        return IP_UNKNOW;
    }

    //判断每个IP段的合法性
    for(int i = 0; i < vecIpSection.size(); i++)
    {
        char* pStop = NULL;
        long int nSection = (long int)strtol(vecIpSection[i].c_str(), &pStop, 10);

        if((int)(pStop - vecIpSection[i].c_str()) != vecIpSection[i].length())
        {
            printf("[Check_IP_V4]error Section[%s].\n", vecIpSection[i].c_str());
            return IP_UNKNOW;
        }

        //printf("[Check_IP_V4]nSection=%d.\n", nSection);
        if(nSection < 0 || nSection > 255)
        {
            return IP_UNKNOW;
        }
    }

    return IP_V4;
}




