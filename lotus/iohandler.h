#ifndef _NET_IOHANDLER_H_
#define _NET_IOHANDLER_H_

class iohandler_t {
public:
    virtual ~iohandler_t() = default;
    virtual int read() = 0;
    virtual int write() = 0;
};

#endif
