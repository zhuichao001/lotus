#pragma once

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
    template<typename REQUEST, typename RESPONSE>
    std::shared_ptr<dialer_t<REQUEST, RESPONSE>> dial(const address_t *addr){
        auto cli = std::make_shared<dialer_t<REQUEST, RESPONSE>>(_ep, addr, this);
        cli->open();
        if(cli->usable()){
            return cli;
        } else {
            return nullptr;
        }
    }

    //boot new server
    template<typename REQUEST, typename RESPONSE>
    int start(const address_t *addr, service_t<REQUEST, RESPONSE>* svr){
        acceptor_t *ac = new acceptor_t(_ep, addr, std::bind(&service_t<REQUEST, RESPONSE>::onconnect, svr, std::placeholders::_1, std::placeholders::_2));//std::bind(&service_t<REQUEST, RESPONSE>::onconnect, svr));
        ac->open();
        _listeners[ac->fd()] = ac;
        return 0;
    }

    void stop();

    int run();

    lotus::timer_t *run_at(timer_callback_t cb, uint64_t when) override;

    lotus::timer_t *run_after(timer_callback_t cb, uint64_t delay) override;
    
    lotus::timer_t *run_every(timer_callback_t cb, uint64_t interval) override;

private:
    bool _stat; //running, closing, closed
    std::map<int, acceptor_t*> _listeners;
    evloop_t *_ep;
    timetracker_t *_tracker;
    bool _running;
};
