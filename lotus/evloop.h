#ifndef _NET_EPOLL_H_
#define _NET_EPOLL_H_

#include <list>
#include <sys/epoll.h>
#include <errno.h>
#include "types.h"
#include "timedriver.h"

class timetracker_t;

class evloop_t {
public:
    evloop_t(const int esize);

    void update(int op, int fd, int events, void *ptr)const;

    int loop();

    void post(task_t t);
private:
    int _efd;
    std::list<task_t> _pendings;
};

#endif
