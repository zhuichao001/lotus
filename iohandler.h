#ifndef _NET_IOHANDLER_H_
#define _NET_IOHANDLER_H_

//abstract for both server and client
class iohandler_t {
public:
    virtual int open(){}
    virtual int close(){}
    virtual int read(){}
    virtual int write(){}
    virtual int handle(){}
    virtual ~iohandler_t(){}
};

#endif
