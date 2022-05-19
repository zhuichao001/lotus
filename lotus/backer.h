#ifndef _NET_BACKER_H_
#define _NET_BACKER_H_

#include <ctime>
#include <functional>
#include "endpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class backer_t:
    public comhandler_t {
public:
    backer_t(evloop_t *ep, int fd, const address_t *addr, service_t *svr):
        _ep(ep),
        _fd(fd),
        _addr(addr),
        _svr(svr){
    }

    int open(){
        _conn = new endpoint_t(SERVER_SIDE, _ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());
        return err;
    }

    int onclose(){
        delete this; //FIXME
        return 0;
    }

    int onreceive(void *request){
        request_t *req = static_cast<request_t*>(request);
        uint64_t msgid = req->msgid();
        fprintf(stderr, "receive msg:%d to process\n", msgid);
        _sessions[msgid] = new session_t(_conn, req); 
        _svr->process(_sessions[msgid]);
        return 0;
    }

private:
    evloop_t *_ep;
    int _fd;
    const address_t *_addr;
    timedriver_t *_watcher;
    endpoint_t *_conn;
    service_t *_svr;
    SessionMap _sessions;
};

#endif
