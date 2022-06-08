#pragma once

#include <map>
#include "callback.h"
#include "timedriver.h"


enum class session_state_t{
    UNKNOWN = 0,
    WAIT_REPLY = 1,
    REPLY_TIMEOUT = 2,
    REPLY_FINISH = 3,
    REPLY_CONNCLOSE = 4,
} ;

template<typename REQUEST, typename RESPONSE>
class endpoint_t;

template<typename REQUEST, typename RESPONSE>
class session_t{
public:
    session_state_t _state;

    endpoint_t<REQUEST, RESPONSE> *_conn;
    REQUEST *_req;
    SessionCallback<REQUEST, RESPONSE> _callback;
    uint64_t  _rpcat; //start time

    session_t(endpoint_t<REQUEST, RESPONSE> *conn, REQUEST *req):
        _conn(conn),
        _req(req){
        _callback = nullptr;
        _rpcat = millisec();
        _state = session_state_t::UNKNOWN;
    }

    REQUEST *request(){
        return _req;
    }

    //for server side
    int reply(RESPONSE *rsp){
        if(_state==session_state_t::REPLY_CONNCLOSE){
            fprintf(stderr, "Failed reply, session state:REPLY_CONNCLOSE\n");
            return -1;
        }

        fprintf(stderr, "reply test 1\n");
        rsp->setmsgid(_req->msgid());
        fprintf(stderr, "reply test 2\n");

        buff_t buf(4096);
        rsp->encode(&buf);
        fprintf(stderr, "reply test 3\n");

        _conn->send(&buf);
        fprintf(stderr, "reply test 4\n");
        return 0;
    }

    //for client side
    int onreply(RESPONSE *rsp){
        if(_callback==nullptr){
            return 0;
        }
        return _callback(_req, rsp); //RPC Response
    }

    bool completed(){
        return _state==session_state_t::REPLY_TIMEOUT || 
                _state==session_state_t::REPLY_CONNCLOSE ||
                _state==session_state_t::REPLY_FINISH;
    }

};
