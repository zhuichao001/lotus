#ifndef _NET_IOHANDLER_H_
#define _NET_IOHANDLER_H_

#include "poll.h"
#include "buff.h"

class iohandler_t {
public:
    iohandler_t(epoll_t *ep, int fd):
        _ep(ep),
        _fd(fd),
        _rb(2048), 
        _wb(4096){
    }
    virtual ~iohandler_t() = default;

    virtual int open() = 0;
    virtual int close() = 0;
    virtual int handle() = 0; //deal incoming data
    virtual int read();

    int send(buff_t *);
    int write();

    int fd(){
        return _fd;
    }

protected:
    epoll_t *_ep;
    int _fd;
    buff_t _rb;
    buff_t _wb;
};

#endif
