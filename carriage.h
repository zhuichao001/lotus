#ifndef _NET_CARRIAGE_H
#define _NET_CARRIAGE_H

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
#include "protocol.h"
#include "client.h"

using namespace std;


//as engine of client
class carriage_t {
public:
    carriage_t(){
        _ep = new epoll_t(MAX_CONN_NUMS);
    }
    ~carriage_t(){
        delete []_ep;
    }
    void done(request_t *req, response_t *res){
        fprintf(stderr, "call done\n");
        return;
    }
    int start(const address_t *addr){
        rpc_client_t *cli = new rpc_client_t(_ep, addr);
        request_t req;

        using namespace std::placeholders;
        cli->call(&req, std::bind(&carriage_t::done, this, _1, _2));
        return 0;
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
