#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

#include <stdio.h>
#include <string.h>
#include "protocol.h"

class service_t {
public:
    virtual int process(request_t *req, response_t *rsp) = 0;
    virtual ~service_t() = default;
};

#endif
