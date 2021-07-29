#ifndef _NET_EPOLL_H_
#define _NET_EPOLL_H_

#include <list>
#include <sys/epoll.h>
#include <errno.h>
#include <memory.h>
#include "task.h"

class iohandler_t;

class epoll_t {
public:
    epoll_t(const int esize){
        _efd = epoll_create(esize);
    }

    void update(int op, int fd, int events, void *ptr)const;

    int loop();

    void post(task_t t);
private:
    int _efd;
    std::list<task_t> pendings;
};

#endif
