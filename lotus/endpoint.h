#ifndef _NET_ENDPOINT_H_
#define _NET_ENDPOINT_H_

#include "iohandler.h"
#include "poll.h"
#include "server.h"
#include "address.h"
#include "buff.h"
#include "socket.h"
#include "endpoint.h"


class endpoint_t: public iohandler_t {
public:
    endpoint_t(epoll_t *ep, int fd, const address_t *addr, server_t *svr):
        iohandler_t(ep, fd), 
        _addr(addr), 
        _svr(svr),
        _rb(2048), 
        _wb(4096){
    }

    virtual ~endpoint_t() = default;

    int open() override;

    int close() override;

    int read() override;

    int write() override;

    int send(buff_t *buf);
private:
    int receive();

    const address_t *_addr;
    server_t *_svr;
    buff_t _rb;
    buff_t _wb;
};

#endif
