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
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t(_ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());
        return err;
    }

    int onclose()override{
        delete this;
        return 0;
    }

    int onreceive(buff_t *buf)override{
        REQUEST req;
        int n = req.decode(buf);
        if(n<0){ //failed
            fprintf(stderr, "Error: request decode failed\n");
            return -1;
        }else if(n==0){ //incomplete
            return 0;
        }else{ //ok
            buf->release(n);
        }

        fprintf(stderr, "receive msg to process\n");
        auto session = new session_t<REQUEST, RESPONSE>(_conn, &req); 
        _processcb(session);
        delete session;

        return 1; //success
    }

private:
    evloop_t *_ep;
    const int _fd;
    address_t _addr;
    timedriver_t *_watcher;
    ProcessCallback<REQUEST, RESPONSE> _processcb;
    endpoint_t *_conn;
};
