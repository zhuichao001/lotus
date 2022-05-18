#ifndef _NET_BACKER_H_
#define _NET_BACKER_H_

#include <ctime>
#include <functional>
#include "endpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class backer_t {
public:
    backer_t(evloop_t *ep, int fd, const address_t *addr, service_t *svr):
        _addr(addr),
        _svr(svr){
        _conn = new endpoint_t(SERVER_SIDE, ep, fd, std::bind(&backer_t::receive, this, std::placeholders::_1));
    }

    int open(){
        return _conn->open();
    }

    int receive(void *request){
        request_t *req = static_cast<request_t*>(request);
        uint64_t msgid = req->msgid();
        _sessions[msgid] = new session_t(_conn->shared_from_this(), req); 
        _svr->process(_sessions[msgid]);
        return 0;
    }

private:
    const address_t *_addr;
    timedriver_t *_watcher;
    endpoint_t *_conn;
    service_t *_svr;
    SessionMap _sessions;
};

#endif
