#ifndef _NET_DIALER_H_
#define _NET_DIALER_H_

#include <ctime>
#include "startpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class dialer_t {
public:
    dialer_t(evloop_t *ep, const address_t *addr, timedriver_t *td):
        _addr(addr),
        _watcher(td){
        _ioh = new startpoint_t(ep, addr, &_sessions);
        _ioh->open();
    }

    void ontimeout(uint64_t msgid){
        fprintf(stderr, "msgid:%ld has been time-out.\n", msgid);
        auto it = _sessions.find(msgid);
        if(it==_sessions.end()){
            return;
        }

        session_t *session = it->second;
        if(!session->completed()){
            response_t rsp;
            rsp.seterrcode(RPC_TIMEOUT);
            session->reply(&rsp);
            fprintf(stderr, "msgid:%ld TIME-OUT.\n", msgid);
            session->_state = session_t::REPLY_TIMEOUT;
        }
    }

    int call(const request_t *req,  RpcCallback callback, uint64_t us=2000000 /*timeout milisec*/){
        uint64_t msgid = req->msgid();

        _sessions[msgid] = new session_t; 
        _sessions[msgid]->_callback = [=](response_t *rsp)->int{ //decorator
            auto it = _sessions.find(msgid);
            if(it==_sessions.end()){
                fprintf(stderr, "msgid:%ld has been erased.\n", msgid);
                return -1;
            }
            if(_sessions[msgid]->completed()){
                fprintf(stderr, "msgid:%ld has been completed.\n", msgid);
                return -1;
            }
            _sessions[msgid]->_state = session_t::REPLY_FINISH;
            fprintf(stderr, "msgid:%ld has replied NORMAL-DATA.\n", msgid);
            return callback(rsp);
        };

        _sessions[msgid]->_state = session_t::WAIT_REPLY;
        _sessions[msgid]->_rpcat = millisec();
        _watcher->run_after(std::bind(&dialer_t::ontimeout, this, msgid), us);

        buff_t buf(2048);
        req->encode(&buf);
        _ioh->send(&buf);
        return 0;
    }

    int fd() {
        return _ioh->fd();
    }

    bool usable(){
        return fd()>0;
    }

private:
    const address_t *_addr;
    timedriver_t *_watcher;
    startpoint_t *_ioh;
    SessionMap _sessions;
};

#endif
