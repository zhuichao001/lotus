#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "address.h"
#include "socket.h"
#include "answer.h"

template<typename REQUEST, typename RESPONSE>
class session_t;

template<typename REQUEST, typename RESPONSE>
class service_t {
public:
    int onconnect(evloop_t *ep, int fd){
        address_t *addr = new address_t;
        get_peer_ip_port(fd, &(addr->ip), &(addr->port));
        answer_t<REQUEST, RESPONSE> *h = new answer_t<REQUEST, RESPONSE>(ep, fd, addr, this->process);
        h->open();
        return 0;
    }

    int (*process)(session_t<REQUEST, RESPONSE> *se);
    ~service_t() = default;
};

#endif
