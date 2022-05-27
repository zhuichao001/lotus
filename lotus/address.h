#pragma once

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

    const std::string to_string()const{
        return std::move(ip+":"+std::to_string(port));
    }

    const uint64_t to_long()const{
        uint64_t ret = 0;
        for (int i = 0; i < ip.length(); i++) {
            if(ip[i]=='.'){
                continue;
            }
            ret = ret*10+(ip[i]-'0');
        }
        ret *= 100000;
        ret += port;
        return ret;
    }

    std::string ip; 
    int port;
};
