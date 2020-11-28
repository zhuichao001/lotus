#ifndef _NET_ENDPOINT_H_
#define _NET_ENDPOINT_H_

#include "iohandler.h"
#include "poll.h"
#include "server.h"
#include "buff.h"


class endpoint_t: public iohandler_t {
public:
    endpoint_t(epoll_t *ep, int fd, server_t *svr):
        _ep(ep), _fd(fd), _svr(svr), _rb(2048), _wb(4096){
    }
    int open();
    int close();
    int read();
    int write();
    int handle();
private:
    int _efd;
    int _fd;
    buff_t _rb;
    buff_t _wb;
    server_t *_svr;
    epoll_t *_ep;
};

#endif
