#pragma once

#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "handler.h"
#include "address.h"
#include "buff.h"
#include "evloop.h"
#include "callback.h"
#include "service.h"

class acceptor_t: public iohandler_t {
public:
    acceptor_t(evloop_t *ep, const address_t *addr, AcceptCallback cb):
        _ep(ep),
        _fd(-1),
        _addr(addr), 
        _acceptcb(cb){
    }

    virtual ~acceptor_t() {
        close();
    }

    int open();

    int close();

    int read() override;

    int write() override { return 0; }

    int fd() { return _fd; }
private:
    const int BACK_LOG_SIZE = 512;

    evloop_t *_ep;
    int _fd;
    const address_t *_addr;
    AcceptCallback _acceptcb;
};
