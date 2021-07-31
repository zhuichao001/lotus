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
          _ep->post(std::bind(&timer_tracker_t::regist, this, when));
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
        if((*it)->expireat() < now) {
            (*it)->run();
            if((*it)->repeatable()){
                futures.push_back((*it)->next());
            }
            it = _timers.erase(it);
        } else {
            break;
        }
    }

    for(auto &it: futures){
        _timers.insert(it);
    }
    return 0;
}

struct timespec time_from_now(uint64_t us ) {
    if (us < 100) {
        us = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>( us / 1000000);
    ts.tv_nsec = static_cast<long>( (us % 1000000) * 1000);
    return ts;
}

int timer_tracker_t::regist(uint64_t expireat){
    struct itimerspec cur, last;
    memset(&cur, 0, sizeof cur);
    memset(&last, 0, sizeof last);
    cur.it_value = time_from_now(expireat - microsec());
    ::timerfd_settime(_timerfd, 0, &cur, &last);
}
