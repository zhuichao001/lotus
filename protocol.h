#ifndef _NET_PROTOCOL_H_
#define _NET_PROTOCOL_H_

#include "buff.h"

class request_t{
public:
    buff_t *encode(){
        return nullptr;
    }

    int decode(buff_t *buff){
        buff->used(&data, &len);
        return len;
    }

    char *data;
    int len;
};

class response_t{
public:
    response_t(){
        buf = new buff_t(2048);
    }

    buff_t *encode(){
        return buf;
    }

    int decode(buff_t *buff){
        return 0;
    }

    buff_t *buf;
};

#endif
