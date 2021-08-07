#ifndef _NET_DIALER_H_
#define _NET_DIALER_H_

#include <ctime>
#include "transport.h"
#include "startpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class dialer_t : public transport_t {
public:
    dialer_t(evloop_t *ep, const address_t *addr):
        _addr(addr){
        _ioh = new startpoint_t(ep, addr, &_sessions);
        _ioh->open();
    }

    int call(const request_t *req,  RpcCallback callback){
        int msgid = req->msgid();

        _sessions[msgid] = new session_t; 
        _sessions[msgid]->_callback = callback;

        buff_t buf(2048);
        req->encode(&buf);
        _ioh->send(&buf);
        return 0;
    }

    int fd() {
        return _ioh->fd();
    }

private:
    const address_t *_addr;
    startpoint_t *_ioh;
    SessionMap _sessions;
};

#endif
