#ifndef _NET_ADDRESS_H_
#define _NET_ADDRESS_H_

typedef struct address_t {
    const char *ip; 
    const int port;
    address_t(const char*s, int d):ip(s),port(d){
    }
} address_t;

#endif
