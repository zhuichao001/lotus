#pragma once

//for engine
class iohandler_t {
public:
    virtual ~iohandler_t() = default;
    virtual int read() = 0;
    virtual int write() = 0;
};

class buff_t;

//for upper layer
class comhandler_t {
public:
    virtual int onreceive(buff_t *) = 0;
    virtual int onclose() = 0;
};
