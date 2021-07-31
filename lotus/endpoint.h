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
        _ep(ep), 
        _fd(fd),
        _addr(addr), 
        _svr(svr),
        _rb(2048), 
        _wb(4096){
    }

    ~endpoint_t(){
        close();
    }

    int open();

    int close();

    int read() override;

    int write() override;

    int send(buff_t *buf);
private:
    int receive();

    epoll_t *_ep;
    int _fd;
    const address_t *_addr;
    server_t *_svr;
    buff_t _rb;
    buff_t _wb;
};

#endif
