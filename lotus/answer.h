#ifndef _NET_BACKER_H_
#define _NET_BACKER_H_

#include <ctime>
#include <functional>
#include "endpoint.h"
#include "address.h"
#include "session.h"
#include "callback.h"
#include "util.h"

using namespace std;


template<typename REQUEST, typename RESPONSE>
class answer_t:
    public comhandler_t {
public:
    answer_t(evloop_t *ep, int fd, const address_t *addr, ProcessCallback<REQUEST, RESPONSE> procb):
        _ep(ep),
        _fd(fd),
        _addr(addr),
        _processcb(procb){
    }

    ~answer_t(){
        for(auto it : _sessions){
            int msgid = it.first;
            delete _sessions[msgid];
        }
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t<REQUEST, RESPONSE>(side_type_t::SERVER_SIDE, _ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());
        return err;
    }

    int onclose(){
        delete this;
        return 0;
    }

    int onreceive(void *request){
        REQUEST *req = static_cast<REQUEST*>(request);
        uint64_t msgid = req->msgid();
        fprintf(stderr, "receive msg:%d to process\n", msgid);
        _sessions[msgid] = new session_t<REQUEST, RESPONSE>(_conn, req); 
        _processcb(_sessions[msgid]);

        delete _sessions[msgid];
        _sessions.erase(msgid);
        return 0;
    }

private:
    evloop_t *_ep;
    int _fd;
    const address_t *_addr;
    timedriver_t *_watcher;
    ProcessCallback<REQUEST, RESPONSE> _processcb;
    endpoint_t<REQUEST, RESPONSE> *_conn;
    SessionMap<REQUEST, RESPONSE> _sessions;
};

#endif
