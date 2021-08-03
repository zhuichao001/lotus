#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "iohandler.h"
#include "timer_tracker.h"
#include "util.h"
#include "evloop.h"


evloop_t::evloop_t(const int esize){
    _efd = epoll_create(esize);
    _tracker = new timer_tracker_t(this); 
}

void evloop_t::update(int op, int fd, int events, void *ptr)const{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.ptr = ptr;
    epoll_ctl(_efd, op, fd, &ev);
}

void evloop_t::post(task_t t){
    _pendings.push_back(t);
}

int evloop_t::loop(){
    struct epoll_event events[1024];
    int n = epoll_wait(_efd, events, 1024, 1); //wait at most 1ms
    if(n==-1){
        return -1;
    }

    for(int i=0; i<n; ++i){
        iohandler_t *h = static_cast<iohandler_t *>(events[i].data.ptr);
        if(events[i].events & EPOLLIN){
            if(h->read() < 0){ //TODO continously do until fail
                delete h;
                continue;
            }
        }

        if(events[i].events & EPOLLOUT){
            if(h->write()<0){
                delete h;
                continue;
            }
        }

        if((events[i].events & EPOLLERR) ||
            (events[i].events & (EPOLLRDHUP|EPOLLHUP))){
            delete h;
        }
    }

    while(!_pendings.empty()){
        task_t routine = _pendings.front();
        _pendings.pop_front();
        int e = routine();
        if(e>0){
            _pendings.push_back(routine);
        }
    }
}


lotus::timer_t *evloop_t::run_at(timer_callback_t cb, uint64_t when){
    return _tracker->add(cb, when, 0);
}

lotus::timer_t *evloop_t::run_after(timer_callback_t cb, uint64_t delay){
    return _tracker->add(cb, microsec()+delay, 0);
}

lotus::timer_t *evloop_t::run_every(timer_callback_t cb, uint64_t interval){
    return _tracker->add(cb, microsec()+interval, interval);
}
