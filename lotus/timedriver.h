#ifndef _NET_TIMEDRIVER_H_
#define _NET_TIMEDRIVER_H_

#include "types.h"
#include "timer.h"

class timedriver_t {
public:
    virtual ~timedriver_t() = default;

    virtual lotus::timer_t *run_at(timer_callback_t cb, uint64_t time_us) =0;
    virtual lotus::timer_t *run_after(timer_callback_t cb, uint64_t us) =0;
    virtual lotus::timer_t *run_every(timer_callback_t cb, uint64_t us) =0;
};

#endif
