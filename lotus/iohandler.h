#ifndef _NET_IOHANDLER_H_
#define _NET_IOHANDLER_H_

#include "poll.h"
#include "buff.h"

class iohandler_t {
public:
    iohandler_t(epoll_t *ep, int fd):
        _ep(ep),
        _fd(fd){
    }
    virtual ~iohandler_t() = default;

    virtual int open() = 0;
    virtual int close() = 0;
    virtual int read() = 0;
    virtual int write() = 0;

    int fd(){
        return _fd;
    }
protected:
    epoll_t *_ep;
    int _fd;
};

#endif
