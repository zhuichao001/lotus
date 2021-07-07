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
        _point = new startpoint_t(ep, addr, &_sessions);
        _point->open();
    }

    int call(request_t *req, std::function<int(request_t *, response_t *)> callback){
        req->msgid = get_nanosec(); //FIXME
        _sessions[req->msgid] = new session_t; 
        _sessions[req->msgid]->_req = req;
        _sessions[req->msgid]->_callback = callback;
        buff_t * data = req->encode();
        _point->send(data);
    }
private:
    const address_t *_addr;
    startpoint_t *_point;
    SessionMap _sessions;
};

#endif
