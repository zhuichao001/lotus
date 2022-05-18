#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

#include <stdio.h>
#include <string.h>

class session_t;

class service_t {
public:
    virtual int process(session_t *se) = 0;
    virtual ~service_t() = default;
};

#endif
