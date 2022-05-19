#ifndef _NET_DIALER_H_
#define _NET_DIALER_H_

#include <ctime>
#include "endpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class dialer_t:
    public comhandler_t {
public:
    dialer_t(evloop_t *ep, const address_t *addr, timedriver_t *td):
        _ep(ep),
        _addr(addr),
        _watcher(td){
    }

    int open(){
        int fd = connect(_addr->ip.c_str(), _addr->port);
        if(fd<0){
            return -1;
        }

        _conn = new endpoint_t(CLIENT_SIDE, _ep, fd, this);
        _conn->open();
        return 0;
    }

    int onclose(){
        delete this; //FIXME
        return 0;
    }

    void ontimeout(uint64_t msgid){
        fprintf(stderr, "msgid:%ld has been time-out.\n", msgid);
        auto it = _sessions.find(msgid);
        if(it==_sessions.end()){
            return;
        }

        fprintf(stderr, "%ld timeout @ %ld\n", msgid, microsec());

        session_t *session = it->second;
        if(!session->completed()){
            response_t rsp;
            rsp.seterrcode(RPC_TIMEOUT);
            session->onreply(&rsp);
            fprintf(stderr, "msgid:%ld TIME-OUT.\n", msgid);
            session->_state = session_t::REPLY_TIMEOUT;
        }
    }

    int call(request_t *req,  RpcCallback callback, uint64_t us=2000000 /*timeout microsec*/){
        uint64_t msgid = req->msgid();

        _sessions[msgid] = new session_t(_conn, req); 
        _sessions[msgid]->_callback = [=](response_t *rsp)->int{ //decorator

            fprintf(stderr, "%ld onreply @ %ld\n", msgid, microsec());
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

        fprintf(stderr, "before %ld call @ %ld\n", msgid, microsec());
        _sessions[msgid]->_state = session_t::WAIT_REPLY;
        _sessions[msgid]->_rpcat = microsec();
        _watcher->run_after(std::bind(&dialer_t::ontimeout, this, msgid), us);

        fprintf(stderr, "%ld call @ %ld\n", msgid, microsec());

        buff_t buf(2048);
        req->encode(&buf);
        _conn->send(&buf);
        return 0;
    }

    int onreceive(void *response){
        response_t *rsp = static_cast<response_t*>(response);
        uint64_t msgid = rsp->msgid();
        auto iter = _sessions.find(msgid);
        if(iter == _sessions.end()){
            return -1;
        }
    
        iter->second->onreply(rsp);
    
        delete iter->second;
        _sessions.erase(msgid);
        return 1;
    }

    int fd() {
        return _conn->fd();
    }

    bool usable(){
        return fd()>0;
    }

private:
    evloop_t *_ep;
    const address_t *_addr;
    timedriver_t *_watcher;
    endpoint_t *_conn;
    SessionMap _sessions;
};

#endif
