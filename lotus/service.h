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
        _process(proc){
    }

    int onconnect(evloop_t *ep, int fd){
        answer_t<REQUEST, RESPONSE> *h = new answer_t<REQUEST, RESPONSE>(ep, fd, this->_process);
        h->open();
        return 0;
    }

    ProcessCallback<REQUEST, RESPONSE> _process;
};
