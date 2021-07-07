#ifndef _NET_ACCEPTION_H_
#define _NET_ACCEPTION_H_

#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "iohandler.h"
#include "address.h"
#include "buff.h"
#include "poll.h"
#include "server.h"

class acceptor_t: public iohandler_t {
public:
    acceptor_t(epoll_t *ep, const address_t *addr, server_t *svr):
        iohandler_t(-1),
        _ep(ep), 
        _addr(addr), 
        _svr(svr){
    }
    int open();
    int close();
    int read();
    int handle(){return 0;}

    int listenfd(){return _svrfd;}
private:
    const int BACK_LOG_SIZE = 512;
    epoll_t *_ep;
    int _svrfd;
    const address_t *_addr;
    server_t *_svr;
};

#endif
