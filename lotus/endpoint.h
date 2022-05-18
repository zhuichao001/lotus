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


typedef std::function<int(void *)> message_callback_t;
typedef std::function<int(void)> close_callback_t;

struct endpoint_callbacks_t {
    message_callback_t on_receive;
    close_callback_t on_close;
};

class endpoint_t: public iohandler_t/*, 
    public std::enable_shared_from_this<endpoint_t> */{
public:
    endpoint_t(SIDE_TYPE side, evloop_t *ep, int fd, endpoint_callbacks_t cbs):
        _side(side),
        _ep(ep), 
        _fd(fd),
        _cbs(cbs),
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
    endpoint_callbacks_t _cbs;

    buff_t _rb;
    buff_t _wb;
};

#endif
