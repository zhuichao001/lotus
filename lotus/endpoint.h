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

typedef std::function<int(void *)> ReceiveCallback;

class endpoint_t: public iohandler_t, 
    public std::enable_shared_from_this<endpoint_t> {
public:
    endpoint_t(SIDE_TYPE side, evloop_t *ep, int fd, ReceiveCallback cb):
        _side(side),
        _ep(ep), 
        _fd(fd),
        _callback(cb),
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
    ReceiveCallback _callback;

    buff_t _rb;
    buff_t _wb;
};

#endif
