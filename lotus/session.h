#ifndef _LOTUS_SESSION_H_
#define _LOTUS_SESSION_H_

#include <map>
#include "protocol.h"
#include "timedriver.h"

class session_t{
public:
    session_t(){
        _rpcat = millisec();
        _state = UNKNOWN;
    }

    int onreply(response_t *rsp){
        if(_callback){
            return _callback(rsp);
        }
    }

    bool completed(){
        return _state==REPLY_TIMEOUT || _state==REPLY_FINISH;
    }

    //request_t *_req;
    RpcCallback _callback;

    uint64_t  _rpcat; //start time

    enum {
        UNKNOWN = 0,
        WAIT_REPLY = 1,
        REPLY_TIMEOUT = 2,
        REPLY_FINISH = 3,
    } _state;
};

typedef std::map<uint64_t, session_t*> SessionMap;

#endif
