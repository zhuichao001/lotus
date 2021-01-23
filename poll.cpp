#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "iohandler.h"
#include "poll.h"


void epoll_t::update(int op, int fd, int events, void *ptr)const{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.ptr = ptr;
    epoll_ctl(_efd, op, fd, &ev);
}

int epoll_t::loop(){
    struct epoll_event events[1024];
    int n = epoll_wait(_efd, events, 1024, 100); //wait at most 100ms
    if(n==-1){
        return -1;
    }

    for(int i=0; i<n; ++i){
        iohandler_t *h = (iohandler_t *)(events[i].data.ptr);
        if(events[i].events & EPOLLIN){
            if(h->read() < 0){
                h->close();
                delete h;
            }else{
                h->handle();
            }
        }

        if(events[i].events & EPOLLOUT){
            if(h->write()<0){
                delete h;
                h->close();
            }
        }

        if((events[i].events & EPOLLERR) ||
            (events[i].events & (EPOLLRDHUP|EPOLLHUP))){
            h->close();
        }
    }
}

