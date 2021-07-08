#ifndef _LOTUS_SESSION_H_
#define _LOTUS_SESSION_H_

#include <functional>
#include <map>
#include "protocol.h"

typedef std::function<int(request_t *, response_t *)> RpcCallback;

class session_t{
public:
    int reply(response_t *rsp){
        if(_callback){
            return _callback(_req, rsp);
        }
    }

    request_t *_req;
    RpcCallback _callback;
};

typedef std::map<long long, session_t*> SessionMap;

#endif
