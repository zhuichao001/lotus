#ifndef _NET_STARTPOINT_H_
#define _NET_STARTPOINT_H_

#include "address.h"
#include "iohandler.h"
#include "poll.h"
#include "session.h"
#include "buff.h"


class startpoint_t: public iohandler_t {
public:
    startpoint_t(epoll_t *ep, const address_t *addr, SessionMap *semap):
        iohandler_t(ep, -1), 
        _addr(addr), 
        _sessions(semap),
        _rb(2048), 
        _wb(4096){
    }

    virtual ~startpoint_t() = default;

    int open() override;

    int close() override;

    int read() override;

    int write() override;

    int send(buff_t *buf);
private:
    int receive();

    const address_t *_addr;
    SessionMap *_sessions;
    buff_t _rb;
    buff_t _wb;
};

#endif
