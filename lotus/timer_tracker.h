#ifndef _NET_TIMERTRACKER_H_
#define _NET_TIMERTRACKER_H_


#include <sys/timerfd.h>
#include <set>
#include "iohandler.h"
#include "timer.h"

class evloop_t;

class timer_tracker_t : public iohandler_t {
public:
    timer_tracker_t(evloop_t *ep);

    lotus::timer_t *add(timer_callback_t cb, uint64_t when, double interval);

    lotus::timer_t *cancel(lotus::timer_t *t);

    int read() override;

    int write() override {return 0;}

private:
    int regist(uint64_t expireat);

    evloop_t *_ep;
    int _timerfd;
    std::set<lotus::timer_t *> _timers;
};

#endif
