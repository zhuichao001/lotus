#ifndef _NET_TIMEDRIVER_H_
#define _NET_TIMEDRIVER_H_

#include "types.h"

class timedriver_t {
public:
    virtual ~timedriver_t() = default;

    virtual int run_at(uint64_t time_us, timer_callback_t cb) =0;
    virtual int run_after(uint64_t us, timer_callback_t cb) =0;
    virtual int run_every(uint64_t us, timer_callback_t cb) =0;
};

#endif
