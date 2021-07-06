//
// Created by 92798 on 2021/7/5.
//

#include "MessageDealer.h"

DNS_HEADER *MessageDealer::getDNSHeader(char *buff) {
    auto *header = new DNS_HEADER();
    char *tmp_ptr = buff;
    u_short t_id = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(t_id);
    u_short flags = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(flags);
    u_short answer_rr = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(flags);
    u_short authority_rr = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(flags);
    u_short additional_rr = ntohs(*(u_short *) tmp_ptr);
    header->ID = t_id;
    header->Flags = flags;
    header->Answer_RR = answer_rr;
    header->Authority_RR = authority_rr;
    header->Additional_RR = additional_rr;
    return header;
}

void MessageDealer::printHeaderAll(DNS_HEADER *header) {
    std::cout << "id: " << header->ID << " flag: " << header->Flags << " answer:" << header->Answer_RR << " auhority:"
              << header->Authority_RR << " additional:" << header->Additional_RR << std::endl;
}

std::string MessageDealer::getHostName(char *buff) {

    buff += 12;//忽略掉中间
    int len = std::strlen(buff);
    char *domain = (char *) malloc((sizeof(char *) * len) + 1);
    strcpy(domain, buff);
    for (int i = 0; i < len; i++) {
        if (domain[i] < 33)
            if (i > 0 && i < len)
                domain[i] = '.';
            else
                domain[i] = ' ';
    }
    std::string domain_str = domain;
    domain_str.erase(0, domain_str.find_first_not_of(" ")).erase(domain_str.find_last_not_of(" ") + 1);
    return domain_str;
}

DNS_QUERY *MessageDealer::getDNSQuery(char buff[]) {
    char *tmp_buff = buff;
    auto *query = new DNS_QUERY();
    std::string domain_str = MessageDealer::getHostName(buff);
    tmp_buff += 12;//忽略掉中间
    int len = std::strlen(tmp_buff);
    tmp_buff += len;
    ++tmp_buff;
    u_short type = ntohs(*(u_short *) tmp_buff);
    tmp_buff += sizeof(type);
    u_short class_ = ntohs(*(u_short *) tmp_buff);
    int query_len = 12 + len + 1 + 2 + 2;
    query->name = domain_str;
    query->type = type;
    query->class_ = class_;
    query->headerAndQueryLength = query_len; //这里是为了读response方便，这个长度包含了头部长度以及query长度，需要receiver直接加这个length得到response主体
    return query;
}

void MessageDealer::printQueryAll(DNS_QUERY *query) {
    std::cout << query->name << " type:" << query->type << " class:" << query->class_ << std::endl;
}

char *MessageDealer::ipv4ToChar(const std::string &str) {
    char *result = new char[8];
    char *strc = new char[strlen(str.c_str()) + 1];
    strcpy(strc, str.c_str());
    char *tmp_id = strtok(strc, ".");
    int len = 0;
    while (tmp_id != nullptr) {
        sprintf(result + len, "%02x", atoi(tmp_id));
        len += 2;
        tmp_id = strtok(nullptr, ".");
    }
    return result;
}

char *MessageDealer::ipv6ToChar(const std::string &str) {
    char *result = new char[32];
    char *strc = new char[strlen(str.c_str()) + 1];
    strcpy(strc, str.c_str());
    char *tmp_id = strtok(strc, ":");
    int len = 0;
    while (tmp_id != nullptr) {
        int str_len= strlen(tmp_id);
        if(str_len==4) {
            sprintf(result + len, "%s", tmp_id);
            len += 4;
        }
        else{
            int padding_len=4-str_len;
            for(;padding_len>0;padding_len--){
                sprintf(result + len, "%x", 0);
                ++len;
            }
            sprintf(result + len, "%s", tmp_id);
            len+=str_len;
        }
        tmp_id = strtok(nullptr, ":");
    }
    return result;
}

std::string MessageDealer::charToIpv6(char *str) {
    char* result_char=new char[40];
    int len= strlen(str);
    int result_len=0;
    for(int i=0;i<len;i=i+4){
        char* tmp=new char[4];
        strncpy(tmp,str,4);
        char* endptr;
        sprintf(result_char+result_len,"%x", strtol(tmp,&endptr,16));
        sprintf(result_char+result_len+1,"%s", ":");
        result_len= strlen(result_char);
        str+=4;
    }
    std::string result=result_char;
    return result;
}

std::string MessageDealer::charToIpv4(char *str) {
    return std::__cxx11::string();
}

unsigned short *MessageDealer::getNewID(unsigned short *recv_ID, sockaddr_in reveice_in, WINBOOL) {
    return nullptr;
}
