#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "socket.h"
#include "protocol.h"
#include "buff.h"
#include "startpoint.h"


int startpoint_t::open(){
    _fd = connect(_addr->ip.c_str(), _addr->port);
    assert(_fd>0);

    set_unblocking(_fd, 1);
    set_reuseaddr(_fd, 1);

    _ep->update(EPOLL_CTL_ADD, _fd, EPOLLIN | EPOLLET, (void*)this);
    return 0;
}

int startpoint_t::close(){
    _wb.reset();
    _rb.reset();

    if(_fd>0){
        _ep->update(EPOLL_CTL_DEL, _fd, EPOLLIN | EPOLLET, (void*)this);
        ::close(_fd);
    }
    _fd = -1;
    return 0;
}

int startpoint_t::handle(){
    fprintf(stderr, "handle is called.\n");

    response_t rsp;
    int n = rsp.decode(&_rb);
    if(n<0){
        fprintf(stderr, "handle rsp.decode fialed.\n");
        return -1;
    }else if(n==0){
        fprintf(stderr, "handle req.decode incomplete.\n");
        return 0;
    }else{
        fprintf(stderr, "handle rsp.decode %d bytes ok.\n", n);
        _rb.release(n);
    }

    uint64_t msgid = rsp.msgid();
    if(_sessions->find(msgid) == _sessions->end()){
        fprintf(stderr, "data msgid:%d not invalid", msgid);
        return -1;
    }

    (*_sessions)[msgid]->done(&rsp);
    return 1;
}
