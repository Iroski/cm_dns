//
// Created by 92798 on 2021/7/5.
//
#include "MessageDealer.h"
extern IDTransform IDTransTable[ID_AMOUNT];
extern int ID_COUNT;

DNS_HEADER *MessageDealer::getDNSHeader(char *buff) {
    auto *header = new DNS_HEADER();
    char *tmp_ptr = buff;
    u_short t_id = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(t_id);
    u_short flags = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(flags);
    u_short question = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(question);
    u_short answer_rr = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(flags);
    u_short authority_rr = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(flags);
    u_short additional_rr = ntohs(*(u_short *) tmp_ptr);
    header->id = t_id;
    header->question = question;
    header->flags = flags;
    header->answer_RR = answer_rr;
    header->authority_RR = authority_rr;
    header->additional_RR = additional_rr;
    return header;
}

std::string MessageDealer::getHostName(char *buff, char *domain_start_ptr) {

    int len = std::strlen(buff);
    int c0label=-1;
    char *domain = (char *) malloc((sizeof(char *) * len) + 1);
    strcpy(domain, buff);
    for (int i = 0; i < len; i++) {
        if(domain[i]==-64){
            c0label=i;
            break;
        }
        if (domain[i] < 33)
            domain[i] = '.';
    }
    std::string domain_str = domain;
    if(c0label!=-1){
        domain_str=domain_str.substr(0,c0label);
        int data_ptr= ntohs(*(u_short *) (buff+c0label))-49152;
        char *data_start_str=domain_start_ptr+data_ptr;
        domain_str.append(".");
        domain_str.append(MessageDealer::getHostName(data_start_str,domain_start_ptr));
    }
    domain_str.erase(0, domain_str.find_first_not_of(".")).erase(domain_str.find_last_not_of(".") + 1);
    return domain_str;
}

DNS_QUERY *MessageDealer::getDNSQuery(char *buff) {
    char *tmp_buff = buff;
    auto *query = new DNS_QUERY();
    tmp_buff += 12;
    std::string domain_str = MessageDealer::getHostName(tmp_buff,buff);
    int len = std::strlen(tmp_buff);
    tmp_buff += len;
    ++tmp_buff;
    u_short type = ntohs(*(u_short *) tmp_buff);
    tmp_buff += sizeof(type);
    u_short class_ = ntohs(*(u_short *) tmp_buff);
    int query_len = 12 + len + 1 + 2 + 2;
    query->name = domain_str;
    if(type==1)
        query->type="IPV4";
    else if(type==28)
        query->type="IPV6";
    else if(type==12)
        query->type="PTR";
    else
        query->type=std::to_string(type);
    query->class_ = class_;
    query->headerAndQueryLength = query_len; //这里是为了读response方便，这个长度包含了头部长度以及query长度，需要receiver直接加这个length得到response主体
    return query;
}

std::vector<DNS_RESPONSE> MessageDealer::getAllResponses(char *ptr, int startPoint, int num) {
    std::vector<DNS_RESPONSE> result;
    char *tmp_buff = ptr + startPoint;
    int *length = new int;
    while (num > 0) {
        --num;
        result.push_back(getResponse(tmp_buff, length,ptr));
        if (num != 0)
            tmp_buff += *length;
    }
    return result;
}

DNS_RESPONSE MessageDealer::getResponse(char *ptr, int *length, char * domain_start_ptr) {
    auto *response = new DNS_RESPONSE();
    char *tmp_ptr = ptr;

    int name_ptr= ntohs(*(u_short *) tmp_ptr)-49152; //c000;
    char *name_start_str=domain_start_ptr+name_ptr;
    response->name=MessageDealer::getHostName(name_start_str,domain_start_ptr);
    tmp_ptr += sizeof(u_short);

    u_short type = ntohs(*(u_short *) tmp_ptr);
    if(type==1)
        response->type="IPV4";
    else if(type==28)
        response->type="IPV6";
    else if(type==5)
        response->type="CNAME";
    else
        response->type=std::to_string(type);
    tmp_ptr += sizeof(u_short);

    response->class_ = ntohs(*(u_short *) tmp_ptr);
    tmp_ptr += sizeof(u_short);

    response->ttl = ntohl(*(u_long *) tmp_ptr);
    tmp_ptr += sizeof(u_long);

    u_short data_length = ntohs(*(u_short *) tmp_ptr);
    response->data_length = data_length;
    tmp_ptr += sizeof(u_short);

    char* data=new char[1024];
    memset(data,0,sizeof(data));
    memcpy(data,tmp_ptr,data_length);
    std::string data_str;
    if(type==1){
        data_str= charToIpv4(data);
    }else if(type==28){
        data_str= charToIpv6(data);
    }else {
        data_str=MessageDealer::getHostName(tmp_ptr,domain_start_ptr);
    }
    response->data=data_str;
    *length=sizeof(u_short)*4+data_length+sizeof(u_long);
    return *response;
}



Message MessageDealer::messageInit(char *ptr,bool isResponse) {
    auto *message = new Message();
    message->setHeader(getDNSHeader(ptr));
    DNS_QUERY* query=getDNSQuery(ptr);
    message->setQuery(query);
    if(isResponse&&(query->type=="IPV4"||query->type=="IPV6")){
        int len = query->headerAndQueryLength;
        message->setResponses(getAllResponses(ptr,len,message->getHeader()->answer_RR));
    }
    return *message;
}

char *MessageDealer::ipv4ToChar(const std::string &str) {
    char *p=(char*)str.data();
    char buf[1024];
    inet_pton(AF_INET,p,buf);
    return buf;
}

char *MessageDealer::ipv6ToChar(const std::string &str) {
    char *p=(char*)str.data();
    char *buf=new char[1024];
    inet_pton(AF_INET6,p,buf);
    return buf;
}

std::string MessageDealer::charToIpv6(char *str) {
    char result_char[40];
    inet_ntop(AF_INET6,str,result_char,40);
    std::string result=result_char;
    return result;
}

std::string MessageDealer::charToIpv4(char *str) {
    char *result_char = new char[40];
    inet_ntop(AF_INET,str,result_char,40);
    std::string result=result_char;
    return result;
}

unsigned short MessageDealer::getNewID(unsigned short recv_ID, sockaddr_in receive_in, BOOL processed) {
    IDTransTable[ID_COUNT].oldID = recv_ID;
    IDTransTable[ID_COUNT].client = receive_in;
    IDTransTable[ID_COUNT].done = processed;
    ID_COUNT++;

    return (unsigned short)(ID_COUNT - 1);
}

void MessageDealer::printResponsesDetailed(const std::vector<DNS_RESPONSE>& responses) {
    int count=1;
    if(responses.empty())
    {
        std::cout<<"No Answer"<<std::endl;
        return;
    }
    for(const auto & response : responses){
        std::cout<<"Response: "<<"count: "<<count<<" name:"<<response.name<<" type:"<<response.type
                     <<" class:"<<response.class_<<" ttl:"<<response.ttl<<" dataLength:"<<response.data_length<<" data:"<<response.data<<std::endl;
        ++count;
    }
}

void MessageDealer::printQueryDetailed(DNS_QUERY *query) {
    std::cout <<"Query: "<<"domain name:"<< query->name << " type:" << query->type << " class:" << query->class_ << " length:"
              << query->headerAndQueryLength << std::endl;
}

void MessageDealer::printHeaderDetailed(DNS_HEADER *header) {
    std::cout <<"Header: "<< "id: " << header->id;
    printf(" 0x%x",header->flags);
    std::cout<< " question: " << header->question << " answer:"
              << header->answer_RR << " authority:"
              << header->authority_RR << " additional:" << header->additional_RR << std::endl;
}

void MessageDealer::printDetailedInfo(Message message) {
    MessageDealer::printHeaderDetailed(message.getHeader());
    MessageDealer::printQueryDetailed(message.getQuery());
    MessageDealer::printResponsesDetailed(message.getResponses());
}

void MessageDealer::printQuerySimple(DNS_QUERY *query) {
    std::cout <<"QuerySimpleInfo:  "<<"domain name:"<< query->name << "  type:" << query->type << std::endl;
}

void MessageDealer::printResponsesSimple(const std::vector<DNS_RESPONSE> &responses) {
    int count = 1;

    if(responses.empty())
    {
        std::cout<<"No Answer"<<std::endl;
        return;
    }
    for (const auto &response : responses) {
        if (response.type == "IPV4" || response.type == "IPV6")
            std::cout << "Response: " << " data: " << response.data << std::endl;
        ++count;
    }
}

bool MessageDealer::isIntercept(Message message) {
    int flag=message.getHeader()->flags;
    return flag%16==3;
}






