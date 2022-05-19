#ifndef _NET_TIMERTRACKER_H_
#define _NET_TIMERTRACKER_H_


#include <sys/timerfd.h>
#include <set>
#include "handler.h"
#include "timer.h"

class evloop_t;

class timetracker_t : public iohandler_t {
public:
    timetracker_t(evloop_t *ep);

    int add(lotus::timer_t* t); 

    int del(lotus::timer_t *t);

    int read() override;

    int write() override {return 0;}

private:
    int appoint(uint64_t expireat);

    evloop_t *_ep;
    int _timerfd;
    std::set<lotus::timer_t *, lotus::TimerCompare> _timers;
};

#endif
