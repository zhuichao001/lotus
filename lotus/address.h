#ifndef _NET_ADDRESS_H_
#define _NET_ADDRESS_H_

#include <string>

class address_t {
public:
    address_t() = default;

    address_t(const char*h, int p):
        ip(h),
        port(p){
    }

    address_t(const address_t &addr){
        ip = addr.ip;
        port = addr.port;
    }

    address_t(std::string &host){
        char tmp[32]={0,};
        sscanf(host.c_str(), "%s:%d", tmp, &port);
        ip = tmp;
    }

    const std::string && to_string()const{
        return std::move(ip+":"+std::to_string(port));
    }

    std::string ip; 
    int port;
};

#endif
