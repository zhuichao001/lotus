#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "socket.h"
#include "protocol.h"
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

int startpoint_t::read(){
    bread(_fd, &_rb);
    _ep->post(std::bind(&startpoint_t::receive, this));
    return 0;
}

int startpoint_t::send(buff_t *buf){
    _wb.append(buf);
    write();
    return 0;
}

int startpoint_t::write(){
    if(!_wb.empty()){
        bwrite(_fd, &_wb);
    }
    if(!_wb.empty()){
        //fprintf(stderr, "fd:%d regist EPOLLOUT with %d byptes\n", _fd, _wb.len());
        _ep->update(EPOLL_CTL_ADD, _fd, EPOLLOUT, (void*)this);
    }
    return 0;
}

int startpoint_t::receive(){
    response_t rsp;
    int n = rsp.decode(&_rb);
    if(n<0){
        //fprintf(stderr, "handle rsp.decode fialed.\n");
        return -1;
    }else if(n==0){
        //fprintf(stderr, "handle req.decode incomplete.\n");
        return 0;
    }else{
        //fprintf(stderr, "handle rsp.decode %d bytes ok.\n", n);
        _rb.release(n);
    }

    uint64_t msgid = rsp.msgid();
    auto iter = _sessions->find(msgid);
    if(iter == _sessions->end()){
        //fprintf(stderr, "data msgid:%d not invalid", msgid);
        return -1;
    }

    iter->second->reply(&rsp);

    delete iter->second;
    _sessions->erase(msgid);

    return 1;
}
