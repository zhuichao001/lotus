#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "socket.h"
#include "protocol.h"
#include "buff.h"
#include "socket.h"
#include "endpoint.h"


int endpoint_t::open(){
    set_unblocking(_fd, 1);
    _ep->update(EPOLL_CTL_ADD, _fd, EPOLLIN | EPOLLET, (void*)this);
    return 0;
}

int endpoint_t::close(){
    _wb.reset();
    _rb.reset();

    _ep->update(EPOLL_CTL_DEL, _fd, 0, nullptr);
    ::close(_fd);
    _fd = -1;
    return 0;
}

int endpoint_t::read(){
    bread(_fd, &_rb);
    _ep->post(std::bind(&endpoint_t::receive, this));
    return 0;
}

int endpoint_t::receive(){
    request_t req;
    int n = req.decode(&_rb);

    int origlen = _rb.len();
    if(n<0){
        //fprintf(stderr, "handle req.decode fialed.\n");
        return -1;
    }else if(n==0){
        //fprintf(stderr, "handle req.decode incomplete.\n");
        return 0;
    }else{
        //fprintf(stderr, "handle req.decode %d bytes ok.\n", n);
        _rb.release(n);
        //fprintf(stderr, "_rb.len:%d after _rb release %d left %d bytes.\n", origlen, n, _rb.len());
    }

    response_t rsp;
    int err = _svr->process(&req, &rsp);

    rsp.seterrcode(err);
    rsp.setmsgid(req.msgid());
    
    buff_t buf(2048);
    rsp.encode(&buf);
    send(&buf);
    return 1;
}

int endpoint_t::send(buff_t *buf){
    _wb.append(buf);
    write();
    return 0;
}

int endpoint_t::write(){
    if(!_wb.empty()){
        bwrite(_fd, &_wb);
    }
    if(!_wb.empty()){
        //fprintf(stderr, "fd:%d regist EPOLLOUT with %d byptes\n", _fd, _wb.len());
        _ep->update(EPOLL_CTL_ADD, _fd, EPOLLOUT, (void*)this);
    }
    return 0;
}
