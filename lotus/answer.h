#pragma once

#include <ctime>
#include <functional>
#include "address.h"
#include "socket.h"
#include "endpoint.h"
#include "session.h"
#include "callback.h"
#include "util.h"


template<typename REQUEST, typename RESPONSE>
class answer_t:
    public comhandler_t {
public:
    answer_t(evloop_t *ep, int fd, ProcessCallback<REQUEST, RESPONSE> procb):
        _ep(ep),
        _fd(fd),
        _processcb(procb){
        get_peer_ip_port(_fd, &(_addr.ip), &(_addr.port));
    }

    ~answer_t(){
        for(auto it : _sessions){
            int msgid = it.first;
            delete _sessions[msgid];
        }
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t<REQUEST, RESPONSE>(endpoint_t<REQUEST, RESPONSE>::SERVER_SIDE, _ep, _fd, this);
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
    const int _fd;
    address_t _addr;
    timedriver_t *_watcher;
    ProcessCallback<REQUEST, RESPONSE> _processcb;
    endpoint_t<REQUEST, RESPONSE> *_conn;
    std::map<uint64_t, session_t<REQUEST, RESPONSE> *> _sessions;
};
