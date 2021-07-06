#ifndef _NET_ENDPOINT_H_
#define _NET_ENDPOINT_H_

#include "address.h"
#include "iohandler.h"
#include "poll.h"
#include "server.h"
#include "buff.h"


class startpoint_t: public iohandler_t {
public:
    startpoint_t(epoll_t *ep, const address_t *addr):
        _ep(ep), _addr(addr), _rb(2048), _wb(4096){
    }

    ~startpoint_t() = default;

    int open();
    int close();
    int read();
    int write();
    int handle();

    int fd() {
        return _fd;
    }

private:
    const int BACK_LOG_SIZE = 512;
    int _efd;
    int _fd;
    buff_t _rb;
    buff_t _wb;
    epoll_t *_ep;
    const address_t *_addr;
};

#endif
