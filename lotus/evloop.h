#pragma once

#include <list>
#include <sys/epoll.h>
#include <errno.h>
#include "types.h"
#include "list.h"
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
    List<task_t> _pendings;
};
