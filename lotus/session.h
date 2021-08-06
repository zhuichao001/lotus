#ifndef _LOTUS_SESSION_H_
#define _LOTUS_SESSION_H_

#include <map>
#include "protocol.h"

class session_t{
public:
    int reply(response_t *rsp){
        if(_callback){
            return _callback(rsp);
        }
    }

    //request_t *_req;
    RpcCallback _callback;
};

typedef std::map<long long, session_t*> SessionMap;

#endif
