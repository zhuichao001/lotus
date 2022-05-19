#ifndef _NET_ENDPOINT_H_
#define _NET_ENDPOINT_H_

#include <memory>
#include "iohandler.h"
#include "evloop.h"
#include "service.h"
#include "address.h"
#include "buff.h"
#include "socket.h"
#include "endpoint.h"

enum SIDE_TYPE{
    CLIENT_SIDE = 1,
    SERVER_SIDE = 2,
};


class comhandler_t {
public:
    virtual int onreceive(void *) = 0;
    virtual int onclose() = 0;
};

class endpoint_t: 
    public iohandler_t {
public:
    endpoint_t(SIDE_TYPE side, evloop_t *ep, int fd, comhandler_t *ch):
        _side(side),
        _ep(ep), 
        _fd(fd),
        _com(ch),
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

    int fd(){return _fd;}

    bool established(){return _fd>0;}
private:
    int receive();

    SIDE_TYPE _side;
    evloop_t *_ep;
    int _fd;
    comhandler_t *_com;

    buff_t _rb;
    buff_t _wb;
};

#endif
