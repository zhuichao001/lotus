#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "handler.h"
#include "timetracker.h"
#include "util.h"
#include "evloop.h"


evloop_t::evloop_t(const int esize){
    _efd = epoll_create(esize);
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
            if(h->read() < 0){
                fprintf(stderr, "EPOLLIN event read() failed \n");
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

    while(true){
        task_t routine;
        if(!_pendings.pop_front(&routine)){
            break;
        }
        int e = routine();
        if(e>0){ //continously deal
            _pendings.push_back(routine);
        }
    }
    return 0;
}
