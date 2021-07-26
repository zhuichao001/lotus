#ifndef _NET_ADDRESS_H_
#define _NET_ADDRESS_H_
#include <string>

class address_t {
public:
    address_t(const char*h, int p):
        ip(h),
        port(p){
    }
    const std::string ip; 
    const int port;
};

#endif
