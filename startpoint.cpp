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
    _fd = connect(_addr->ip, _addr->port);
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
    char *data = nullptr;
    int len = 0;
    while(true){
        _rb.rest(&data, &len);
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
    _rb.used(&data, &len);
    fprintf(stdout, "%d totally read:%s\n", _fd, data);
    return 0;
}

int startpoint_t::write(){
    while(!_wb.empty()){
        char *data = nullptr;
        int len = 0;
        _wb.used(&data, &len); 

        int n = ::write(_fd, (void *)data, (size_t)len);
        if (n<0 && errno == EAGAIN) { //tcp buffer is full
            break;
        } else if (n<=0) { //error
            fprintf(stderr,"client: write errno:%d.\n", errno);
            return -1;
        } else {
            this->_wb.finish(n);
        }
    }
    return 0;
}

int startpoint_t::handle(){
    fprintf(stderr, "handle is called.\n");

    response_t rsp;
    int n = rsp.decode(&_rb);
    if(n<0){
        return -1;
    }else if(n==0){
        return 0;
    }else{
        fprintf(stderr, "_rb.release.\n", errno);
        _rb.release(n);
    }

    //request_t *req = sessions.request(rsp.msgid);
    //TODO
    return 0;
}
