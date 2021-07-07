#ifndef _NET_ENDPOINT_H_
#define _NET_ENDPOINT_H_

#include "iohandler.h"
#include "poll.h"
#include "server.h"
#include "buff.h"


class endpoint_t: public iohandler_t {
public:
    endpoint_t(epoll_t *ep, int fd, server_t *svr):
        iohandler_t(fd),
        _ep(ep), 
        _svr(svr){
    }

    ~endpoint_t() = default;

    int open();
    int close();
    int handle(); //for user call

private:
    server_t *_svr;
    epoll_t *_ep;
};

#endif
