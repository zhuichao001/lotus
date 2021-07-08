#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

#include <stdio.h>
#include <string.h>
#include "protocol.h"

class server_t {
public:
    virtual int process(request_t *req, response_t *rsp) = 0;
    virtual ~server_t(){}
};

class rpc_server_t: public server_t{
public:
    int process(request_t *req, response_t *rsp){
        fprintf(stderr, "rpc server process.\n"); 
        fprintf(stderr, "rpc req=%s.\n", req->data()); 
        const char * s = "ECHO::";
        rsp->setbody(s, strlen(s));
        rsp->setbody(req->data(), req->len());
        return 0;
    }
};

#endif
