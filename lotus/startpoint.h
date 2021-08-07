#ifndef _NET_STARTPOINT_H_
#define _NET_STARTPOINT_H_

#include "address.h"
#include "iohandler.h"
#include "evloop.h"
#include "session.h"
#include "buff.h"


class startpoint_t: public iohandler_t {
public:
    startpoint_t(evloop_t *ep, const address_t *addr, SessionMap *semap):
        _ep(ep), 
        _fd(-1),
        _addr(addr), 
        _sessions(semap),
        _rb(2048), 
        _wb(4096){
    }

    ~startpoint_t() {
        close();
    }

    int fd(){return _fd;}

    int open();

    int close();

    int read() override;

    int write() override;

    int send(buff_t *buf);

    bool established(){return _fd>0;}
private:
    int receive();

    evloop_t *_ep;
    int _fd;
    const address_t *_addr;
    SessionMap *_sessions;
    buff_t _rb;
    buff_t _wb;
};

#endif
