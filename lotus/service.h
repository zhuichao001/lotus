#pragma once

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "address.h"
#include "socket.h"
#include "answer.h"
#include "callback.h"

template<typename REQUEST, typename RESPONSE>
class service_t {
public:
    service_t(ProcessCallback<REQUEST, RESPONSE> proc):
        process(proc){
    }

    int onconnect(evloop_t *ep, int fd){
        address_t *addr = new address_t;
        get_peer_ip_port(fd, &(addr->ip), &(addr->port));
        answer_t<REQUEST, RESPONSE> *h = new answer_t<REQUEST, RESPONSE>(ep, fd, addr, this->process);
        h->open();
        return 0;
    }

    ProcessCallback<REQUEST, RESPONSE> process;
};
