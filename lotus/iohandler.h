#ifndef _NET_IOHANDLER_H_
#define _NET_IOHANDLER_H_

#include "buff.h"

class iohandler_t {
public:
    iohandler_t(int fd):
        _fd(fd),
        _rb(2048), 
        _wb(4096){
    }
    virtual ~iohandler_t() = default;

    virtual int open() = 0;
    virtual int close() = 0;
    virtual int read();
    virtual int handle() = 0; //unpack incoming data

    int write();
    int send(buff_t *);

    int _fd;
    buff_t _rb;
    buff_t _wb;
};

#endif
