#ifndef _LOTUS_SESSION_H_
#define _LOTUS_SESSION_H_

#include <map>
#include "protocol.h"
#include "timedriver.h"

class session_t{
public:
    enum {
        UNKNOWN = 0,
        WAIT_REPLY = 1,
        REPLY_TIMEOUT = 2,
        REPLY_FINISH = 3,
        REPLY_CONNCLOSE = 3,
    } _state;

    endpoint_t *_conn;
    request_t *_req;
    RpcCallback _callback;
    uint64_t  _rpcat; //start time

    session_t(endpoint_t *conn, request_t *req):
        _conn(conn),
        _req(req){
        _callback = nullptr;
        _rpcat = millisec();
        _state = UNKNOWN;
    }

    request_t *request(){
        return _req;
    }

    //for server side
    int reply(response_t *rsp){
        if(_state==REPLY_CONNCLOSE){
            return -1;
        }

        rsp->setmsgid(_req->msgid());

        buff_t buf(2048);
        rsp->encode(&buf);
        _conn->send(&buf);
        return 0;
    }

    //for client side
    int onreply(response_t *rsp){
        if(_callback==nullptr){
            return 0;
        }
        return _callback(_req, rsp); //RPC Response
    }

    bool completed(){
        return _state==REPLY_TIMEOUT || 
                _state==REPLY_CONNCLOSE ||
                _state==REPLY_FINISH;
    }

};

typedef std::map<uint64_t, session_t*> SessionMap;

#endif
