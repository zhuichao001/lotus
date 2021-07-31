#include <vector>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <unistd.h>
#include "util.h"
#include "evloop.h"
#include "timer_tracker.h"

timer_tracker_t::timer_tracker_t(evloop_t *ep):
    _ep(ep){
    _timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    assert(_timerfd>0);
    _ep->update(EPOLL_CTL_ADD, _timerfd, EPOLLIN | EPOLLET, (void*)this);
}

lotus::timer_t *timer_tracker_t::add(timer_callback_t cb, uint64_t when, double interval) { 
    lotus::timer_t* t = new lotus::timer_t(cb, when, interval);
    _timers.insert(t);
    if(_timers.size()==1 || when<(*_timers.begin())->expireat()){
        reset(when);
    }
    return t;
}

lotus::timer_t *timer_tracker_t::cancel(lotus::timer_t *t){
    auto it = _timers.find(t);
    if(it==_timers.end()){
        return nullptr;
    }else{
        (*it)->cancel();
    }
    return t;
}

int timer_tracker_t::read(){
    uint64_t howmany;
    ssize_t n = ::read(_timerfd, &howmany, sizeof howmany);
    uint64_t now = microsec();

    std::vector<lotus::timer_t *> expired;
    std::vector<lotus::timer_t *> futures;
    
    for(auto it=_timers.begin(); it!=_timers.end(); ){
        if((*it)->expireat() > now) {
            break;
        }

        (*it)->run();
        if((*it)->repeatable()){
            futures.push_back((*it)->next());
        }
        it = _timers.erase(it);
    }

    for(auto &it: futures){
        _timers.insert(it);
    }

    if(!_timers.empty()){
        reset((*_timers.begin())->expireat());
    }
    return 0;
}

int timer_tracker_t::reset(uint64_t expireat){
    struct itimerspec cur;
    memset(&cur, 0, sizeof cur);
    uint64_t after = expireat - microsec();
    struct timespec ts{
        .tv_sec = static_cast<time_t>( after / 1000000),
        .tv_nsec = static_cast<long>( (after % 1000000) * 1000)
    };
    cur.it_value =  ts;
    ::timerfd_settime(_timerfd, 0, &cur, nullptr);
    return 0;
}
