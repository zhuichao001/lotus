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
    int n = epoll_wait(_efd, events, 1024, 1); //wait at most 1ms
    if(n==-1){
        return -1;
    }

    for(int i=0; i<n; ++i){
        iohandler_t *h = static_cast<iohandler_t *>(events[i].data.ptr);
        if(events[i].events & EPOLLIN){
            if(h->read() < 0){
                h->close();
                delete h;
                continue;
            }else{
                pendings.push_back(h);
            }
            update(EPOLL_CTL_ADD, h->fd(), EPOLLOUT, (void*)h);
        }

        if(events[i].events & EPOLLOUT){
            fprintf(stderr, "EPOLLOUT, fd:%d\n", h->fd());
            if(h->write()<0){
                delete h;
                h->close();
                continue;
            }
        }

        if((events[i].events & EPOLLERR) ||
            (events[i].events & (EPOLLRDHUP|EPOLLHUP))){
            h->close();
        }
    }

    while(!pendings.empty()){
        iohandler_t *h = pendings.front();
        pendings.pop_front();
        int e = h->handle();
        fprintf(stderr, "handle return :%d\n", e);
        if(e>0){
            pendings.push_back(h);
        }
    }
}

