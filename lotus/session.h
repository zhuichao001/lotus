#ifndef _LOTUS_SESSION_H_
#define _LOTUS_SESSION_H_

#include <functional>
#include <map>
#include "protocol.h"

class session_t{
public:
    request_t *_req;
    std::function<int(request_t *, response_t *)> _callback;
    int done(response_t *rsp){
        if(_callback){
            return _callback(_req, rsp);
        }
    }
};

typedef std::map<long long, session_t*> SessionMap;

#endif
