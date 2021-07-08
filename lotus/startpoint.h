#ifndef _NET_ENDPOINT_H_
#define _NET_ENDPOINT_H_

#include "address.h"
#include "iohandler.h"
#include "poll.h"
#include "server.h"
#include "session.h"
#include "buff.h"


class startpoint_t: public iohandler_t {
public:
    startpoint_t(epoll_t *ep, const address_t *addr, SessionMap *semap):
        iohandler_t(ep, -1),
        _addr(addr), 
        _sessions(semap){
    }

    ~startpoint_t() = default;

    int open();
    int close();
    int handle();

private:
    const address_t *_addr;
    SessionMap *_sessions;
};

#endif
