#ifndef _NET_TIMER_H_
#define _NET_TIMER_H_

#include "types.h"

class timer_tracker_t;

namespace lotus {

class timer_t {
public:
    timer_t(timer_tracker_t* trac, timer_callback_t cb, uint64_t when, uint64_t interval);

    void fired();

    int cancel();

    bool repeatable()const;

    timer_t *next();

    uint64_t expireat()const;

    int update(uint64_t expireat);

private:
    timer_tracker_t* _tracker;
    const timer_callback_t _callback;
    uint64_t _expire;
    uint64_t _interval;
    bool _canceled;
    bool _repeatable;
};

//Timer poninter compare
class TimerCompare {
public:
    bool operator()(const timer_t *left, const timer_t *right) {
        return left->expireat() < right->expireat();
    }
};

}; //end of namespace lotus

#endif
