#ifndef _NET_BACKER_H_
#define _NET_BACKER_H_

#include <ctime>
#include <functional>
#include "endpoint.h"
#include "address.h"
#include "session.h"
#include "util.h"

using namespace std;

class answer_t:
    public comhandler_t {
public:
    answer_t(evloop_t *ep, int fd, const address_t *addr, service_t *svr):
        _ep(ep),
        _fd(fd),
        _addr(addr),
        _svr(svr){
    }

    ~answer_t(){
        for(auto it : _sessions){
            int msgid = it.first;
            delete _sessions[msgid];
        }
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t<rpc_request_t, rpc_response_t>(side_type_t::SERVER_SIDE, _ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());
        return err;
    }

    int onclose(){
        delete this;
        return 0;
    }

    int onreceive(void *request){
        rpc_request_t *req = static_cast<rpc_request_t*>(request);
        uint64_t msgid = req->msgid();
        fprintf(stderr, "receive msg:%d to process\n", msgid);
        _sessions[msgid] = new session_t(_conn, req); 
        _svr->process(_sessions[msgid]);

        delete _sessions[msgid];
        _sessions.erase(msgid);
        return 0;
    }

private:
    evloop_t *_ep;
    int _fd;
    const address_t *_addr;
    timedriver_t *_watcher;
    endpoint_t<rpc_request_t, rpc_response_t> *_conn;
    service_t *_svr;
    SessionMap _sessions;
};

#endif
