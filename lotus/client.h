#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_

#include <ctime>
#include "transport.h"
#include "startpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class rpc_client_t : public transport_t {
public:
    rpc_client_t(epoll_t *ep, const address_t *addr):_addr(addr){
        _ioh = new startpoint_t(ep, addr, &_sessions);
        _ioh->open();
    }

    int call(request_t *req,  RpcCallback callback){
        int msgid = req->msgid();

        _sessions[msgid] = new session_t; 
        _sessions[msgid]->_req = req;
        _sessions[msgid]->_callback = callback;

        buff_t buf(1024);
        req->encode(&buf);
        fprintf(stderr, "request encode, len:%d\n", buf.len());
        _ioh->send(&buf);
        return 0;
    }

private:
    const address_t *_addr;
    startpoint_t *_ioh;
    SessionMap _sessions;
};

#endif
