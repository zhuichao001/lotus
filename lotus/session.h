#ifndef _LOTUS_SESSION_H_
#define _LOTUS_SESSION_H_

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
            return -1;
        }

        rsp->setmsgid(_req->msgid());

        buff_t buf(2048);
        rsp->encode(&buf);
        _conn->send(&buf);
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

//template<typename REQUEST, typename RESPONSE>
//using SessionMap = std::map<uint64_t, session_t<REQUEST, RESPONSE>*> ;

#endif
