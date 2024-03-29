#pragma once

#include <memory>
#include "handler.h"
#include "evloop.h"
#include "service.h"
#include "address.h"
#include "buff.h"
#include "socket.h"
#include "handler.h"

class endpoint_t: 
    public iohandler_t {
public:
    endpoint_t(evloop_t *ep, int fd, comhandler_t *ch):
        _ep(ep), 
        _fd(fd),
        _com(ch),
        _rb(2048), 
        _wb(4096){
    }

    int open(){
        assert(_fd>0);
        set_reuseaddr(_fd, 1);
        set_unblocking(_fd, 1);
        _ep->update(EPOLL_CTL_ADD, _fd, EPOLLIN | EPOLLET, (void*)this);
        return 0;
    }

    int close(){
        _wb.reset();
        _rb.reset();

        fprintf(stderr, "endpoint closed, fd:%d\n", _fd);

        if(_fd>0){
            _ep->update(EPOLL_CTL_DEL, _fd, 0, nullptr);
            ::close(_fd);
        }
        _fd = -1;

        _com->onclose();
        return 0;
    }

    int read() override {
        bread(_fd, &_rb);
        _ep->post(std::bind(&comhandler_t::onreceive, _com, &_rb));
        return 0;
    }

    int write() override{
        if(_wb.empty()){
            fprintf(stderr, "Warning: write empty\n");
            return -1;
        }

        if(!_wb.empty()){
            bwrite(_fd, &_wb);
        }
        if(!_wb.empty()){
            _ep->update(EPOLL_CTL_ADD, _fd, EPOLLOUT, (void*)this);
        }
        return 0;
    }

    int send(buff_t *buf){
        assert(buf!=nullptr);
        _wb.append(buf);
        write();
        return 0;
    }

    int fd(){
        return _fd;
    }

    bool established(){
        return _fd>0;
    }

private:
    evloop_t *_ep;
    int _fd;
    comhandler_t *_com;

    buff_t _rb;
    buff_t _wb;
};

