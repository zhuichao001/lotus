#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_

#include <ctime>
#include <map>
#include "transport.h"
#include "startpoint.h"
#include "protocol.h"
#include "address.h"
#include "util.h"

using namespace std;

class session_t{
public:
    request_t *_req;
    std::function<void(request_t *, response_t *)> _callback;
};

class rpc_client_t : public transport_t {
public:
    rpc_client_t(epoll_t *ep, const address_t *addr):_addr(addr){
        _point = new startpoint_t(ep, addr);
        _point->open();
    }

    int call(request_t *req, std::function<void(request_t *, response_t *)> callback){
        req->msgid = get_nanosec();
        _sessions[req->msgid] = new session_t; 
        _sessions[req->msgid]->_req = req;
        _sessions[req->msgid]->_callback = callback;
        //data = req.encode(req);
        //_point->write(data);
    }
private:
    const address_t *_addr;
    startpoint_t *_point;
    std::map<long long, session_t*> _sessions;
};

#endif
