#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include "protocol.h"
#include <functional>

class transport_t {
public:
    virtual int call(request_t *req, RpcCallback callback) = 0;
    virtual ~transport_t(){
    }
};

#endif
