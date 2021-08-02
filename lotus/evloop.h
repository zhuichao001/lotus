#ifndef _NET_EPOLL_H_
#define _NET_EPOLL_H_

#include <list>
#include <sys/epoll.h>
#include <errno.h>
#include "types.h"
#include "timedriver.h"

class timer_tracker_t;

class evloop_t : public timedriver_t{
public:
    evloop_t(const int esize);

    void update(int op, int fd, int events, void *ptr)const;

    int loop();

    lotus::timer_t *run_at(uint64_t time_us, timer_callback_t cb) override;

    lotus::timer_t *run_after(uint64_t us, timer_callback_t cb) override;

    lotus::timer_t *run_every(uint64_t us, timer_callback_t cb) override;

    void post(task_t t);
private:
    int _efd;
    timer_tracker_t *_tracker;
    std::list<task_t> _pendings;
};

#endif
