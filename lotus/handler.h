#ifndef _NET_IOHANDLER_H_
#define _NET_IOHANDLER_H_

//for engine
class iohandler_t {
public:
    virtual ~iohandler_t() = default;
    virtual int read() = 0;
    virtual int write() = 0;
};

//for upper layer
class comhandler_t {
public:
    virtual int onreceive(void *) = 0;
    virtual int onclose() = 0;
};

#endif
