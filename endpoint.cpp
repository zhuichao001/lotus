#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "protocol.h"
#include "buff.h"
#include "endpoint.h"


int endpoint_t::open(){
    _ep->update(EPOLL_CTL_ADD, _fd, EPOLLIN|EPOLLOUT, (void*)this);
    return 0;
}

int endpoint_t::close(){
    _ep->update(EPOLL_CTL_DEL, _fd, 0, nullptr);
    _wb.reset();
    _rb.reset();
    ::close(_fd);
    return 0;
}

int endpoint_t::read(){
    char *data = nullptr;
    int len = 0;
    while(true){
        _rb.writable(&data, &len);
        int n = ::read(_fd, data, len);
        if(n<0 && errno==EAGAIN){// read done
            fprintf(stderr, "%d read again.\n", _fd);
            return 0;
        }else if(n==0){   //conn closed
            fprintf(stderr, "%d read closed.\n", _fd);
            return -1;
        }else if(n<0){    //read error
            fprintf(stderr, "%d read errno:%d.\n", _fd, errno);
            return -1;
        }

        _rb.move(0, n);
        if(n<len){  //normal
            fprintf(stdout,"%d read done.\n", _fd);
            break;
        }else if(len-n==0){
            _rb.expand();
        }
    }
    fprintf(stdout, "%d totally read:%s\n", _fd, data);
    return 0;
}

int endpoint_t::write(){
    while(!_wb.empty()){
        char *data = nullptr;
        int len = 0;
        _wb.readable(&data, &len); 

        int n = ::write(_fd, (void *)data, (size_t)len);
        if (n<0 && errno == EAGAIN) { //tcp buffer is full
            break;
        } else if (n<=0) { //error
            fprintf(stderr,"client: write errno:%d.\n", errno);
            return -1;
        } 
        _rb.move(n, 0);
    }
    return 0;
}

int endpoint_t::handle(){
    request_t req;
    int err = req.decode(&_rb);
    if(err<0){
        return -1;
    }

    response_t res;
    int result = _svr->process(&req, &res);
    res.decode(&_wb);
    this->write();
    return 0;
}
