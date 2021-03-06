#ifndef _NET_CARRIAGE_H
#define _NET_CARRIAGE_H

#include <assert.h>
#include <map>

#include "address.h"
#include "poll.h"
#include "protocol.h"
#include "client.h"

using namespace std;


//as entrance api of client
class dialer_t {
public:
    dialer_t(){
        _ep = new epoll_t(MAX_CONN_NUMS);
    }

    ~dialer_t(){
        delete []_ep;
    }

    rpc_client_t * open(const address_t *addr){
        rpc_client_t *cli = new rpc_client_t(_ep, addr);
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
    map<int, rpc_client_t*> _clients;
    epoll_t *_ep;
};

#endif
