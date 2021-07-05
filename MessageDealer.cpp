//
// Created by 92798 on 2021/7/5.
//

#include "MessageDealer.h"

DNS_HEADER* MessageDealer::getDNSHeader(char *buff) {
    auto* header=new DNS_HEADER();
    char* tmp_ptr=buff;
    u_short t_id= ntohs(*(u_short *)tmp_ptr);
    tmp_ptr+=sizeof(t_id);
    u_short flags=ntohs(*(u_short *)tmp_ptr);
    tmp_ptr+=sizeof (flags);
    u_short answer_rr=ntohs(*(u_short *)tmp_ptr);
    tmp_ptr+=sizeof (flags);
    u_short authority_rr=ntohs(*(u_short *)tmp_ptr);
    tmp_ptr+=sizeof (flags);
    u_short additional_rr=ntohs(*(u_short *)tmp_ptr);
    header->ID=t_id;
    header->Flags=flags;
    header->Answer_RR=answer_rr;
    header->Authority_RR=authority_rr;
    header->Additional_RR=additional_rr;
    return header;
}

void MessageDealer::printHeaderAll(DNS_HEADER* header) {
    std::cout<<"id: "<<header->ID<<" flag: "<<header->Flags<<" answer:"<<header->Answer_RR<<" auhority:"<<header->Authority_RR<<" additional:"<<header->Additional_RR<<std::endl;
}

std::string MessageDealer::getHostName(char *buff) {

    buff+=12;//忽略掉中间
    int len=std::strlen(buff);
    char* domain=(char *)malloc((sizeof(char*)*len) + 1);
    strcpy(domain,buff);
    for(int i=0;i<len;i++){
        if(domain[i]<33)
            if(i>0&&i<len)
                domain[i]='.';
            else
                domain[i]=' ';
    }
    std::string domain_str=domain;
    domain_str.erase(0,domain_str.find_first_not_of(" ")).erase(domain_str.find_last_not_of(" ") + 1);
    return domain_str;
}

DNS_QUERY *MessageDealer::getDNSQuery(char buff[]) {
    char* tmp_buff=buff;
    auto* query=new DNS_QUERY();
    std::string domain_str=MessageDealer::getHostName(buff);
    tmp_buff+=12;//忽略掉中间
    int len=std::strlen(tmp_buff);
    tmp_buff+=len;
    ++tmp_buff;
    u_short type=ntohs(*(u_short *)tmp_buff);
    tmp_buff+=sizeof(type);
    u_short class_=ntohs(*(u_short *)tmp_buff);
    int query_len=12+len+1+2+2;
    query->name=domain_str;
    query->type=type;
    query->class_=class_;
    query->headerAndQueryLength=query_len; //这里是为了读reponse方便，这个长度包含了头部长度以及query长度，需要receiver直接加这个length得到response主体
    return query;
}

void MessageDealer::printQueryAll(DNS_QUERY *query) {
    std::cout<<query->name<<" type:"<<query->type<<" class:"<<query->class_<<std::endl;
}
