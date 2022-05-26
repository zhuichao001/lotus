#ifndef _NET_DIALER_H_
#define _NET_DIALER_H_

#include <ctime>
#include "callback.h"
#include "endpoint.h"
#include "address.h"
#include "session.h"
#include "protocol/rpc.h"
#include "util.h"

using namespace std;


template<typename REQUEST, typename RESPONSE>
class dialer_t:
    public comhandler_t,
    public std::enable_shared_from_this<dialer_t<REQUEST, RESPONSE>> {
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

        _conn = new endpoint_t<REQUEST, RESPONSE>(endpoint_t<REQUEST, RESPONSE>::CLIENT_SIDE, _ep, fd, this);
        _conn->open();
        return 0;
    }

    int onclose(){
        for(auto it : _sessions){
            int msgid = it.first;
            session_t<REQUEST, RESPONSE> *session = it.second;
            rpc_response_t rsp;
            rsp.seterrcode(RPC_ERR_CONNCLOSE);
            fprintf(stderr, "incomplete msgid:%ld CONN-CLOSE.\n", msgid);
            session->onreply(&rsp);

            delete session;
        }

        SessionMap<REQUEST, RESPONSE> _;
        _.swap(_sessions);
        return 0;
    }

    void ontimeout(uint64_t msgid){
        auto it = _sessions.find(msgid);
        if(it==_sessions.end()){
            return;
        }

        fprintf(stderr, "msgid:%ld time-out @ %ld\n", msgid, microsec());
        session_t<REQUEST, RESPONSE> *session = it->second;
        if(!session->completed()){
            rpc_response_t rsp;
            rsp.seterrcode(RPC_ERR_TIMEOUT);
            session->onreply(&rsp);
        }

        delete session;
        _sessions.erase(it);
    }

    int onreceive(void *response){
        RESPONSE *rsp = static_cast<RESPONSE*>(response);
        const uint64_t msgid = rsp->msgid();
        auto it = _sessions.find(msgid);
        if(it == _sessions.end()){
            return -1;
        }
        session_t<REQUEST, RESPONSE> *session = it->second;
        session->onreply(rsp);

        delete session;
        _sessions.erase(it);
        return 0;
    }

    int call(REQUEST *req,  SessionCallback<REQUEST, RESPONSE> callback, uint64_t us=4000000 /*timeout microsec*/){
        uint64_t msgid = req->msgid();

        _sessions[msgid] = new session_t<REQUEST, RESPONSE>(_conn, req); 
        _sessions[msgid]->_state = session_state_t::WAIT_REPLY;
        _sessions[msgid]->_rpcat = microsec();
        _sessions[msgid]->_callback = [=](REQUEST *req, RESPONSE *rsp)->int{ //decorator
            auto it = _sessions.find(msgid);
            if(it==_sessions.end()){
                fprintf(stderr, "msgid:%ld has been erased.\n", msgid);
                return -1;
            }

            session_t<REQUEST, RESPONSE> *session = it->second;
            if(session->completed()){
                fprintf(stderr, "msgid:%ld has been completed.\n", msgid);
                return -1;
            }
            session->_state = session_state_t::REPLY_FINISH;
            int err =  callback(req, rsp);
            return err;
        };

        _watcher->run_after(std::bind(&dialer_t<REQUEST, RESPONSE>::ontimeout, this->shared_from_this(), msgid), us);

        fprintf(stderr, "dilaer call msg:%ld @ %ld\n", msgid, microsec());

        buff_t buf(2048);
        req->encode(&buf);
        _conn->send(&buf);
        return 0;
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
    endpoint_t<REQUEST, RESPONSE> *_conn;
    SessionMap<REQUEST, RESPONSE> _sessions;
};

#endif
