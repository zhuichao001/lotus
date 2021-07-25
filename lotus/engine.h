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
#include "poll.h"
#include "acceptor.h"
#include "server.h"
#include "dialer.h"

using namespace std;


class engine_t {
public:
    engine_t(){
        _ep = new epoll_t(MAX_CONN_NUMS);
    }

    ~engine_t(){
        delete []_ep;
    }

    //boot new server
    int start(const address_t *addr, server_t* svr){
        acceptor_t *ac = new acceptor_t(_ep, addr, svr);
        ac->open();
        _listeners[ac->listenfd()] = ac;
        return 0;
    }

    //boot new client
    dialer_t * open(const address_t *addr){
        dialer_t *cli = new dialer_t(_ep, addr);
        return cli;
    }

    int run(){
        while (true) {
            _ep->loop();
        }
    }
private:
    const int MAX_CONN_NUMS  = 1024;
    bool _stat; //running, closing, closed
    map<int, acceptor_t*> _listeners;
    map<int, dialer_t*> _clients;
    epoll_t *_ep;
};

#endif
