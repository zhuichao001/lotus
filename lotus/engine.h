#ifndef _NET_ENGINE_H
#define _NET_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <map>
#include <set>

#include "address.h"
#include "evloop.h"
#include "acceptor.h"
#include "service.h"
#include "dialer.h"
#include "timer.h"
#include "timetracker.h"

const int MAX_CONN_NUMS  = 1024;

class engine_t : public timedriver_t{
public:
    engine_t();

    ~engine_t();

    void heartbeat();

    //boot new client
    dialer_t * open(const address_t *addr);

    //boot new server
    int start(const address_t *addr, service_t* svr);

    void stop();

    int run();

    lotus::timer_t *run_at(timer_callback_t cb, uint64_t when) override;

    lotus::timer_t *run_after(timer_callback_t cb, uint64_t delay) override;
    
    lotus::timer_t *run_every(timer_callback_t cb, uint64_t interval) override;

private:
    bool _stat; //running, closing, closed
    std::map<int, acceptor_t*> _listeners;
    std::map<int, dialer_t*> _clients;
    evloop_t *_ep;
    timetracker_t *_tracker;
    bool _running;
};

#endif
