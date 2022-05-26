#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include "protocol/rpc.h"
#include "engine.h"
#include "dialer.h"

int rpcdone(rpc_request_t *req, rpc_response_t *rsp){
    if(rsp->errcode()!=0){
        fprintf(stderr, "rpc failed, response error code:[%d]\n", rsp->errcode());
        return -1;
    }
    fprintf(stderr, "call rpc done, response:[%s]\n", rsp->data());
    delete req;
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc<2){
        fprintf(stderr, "usage: -n [rpc count]\n");
        return 0;
    }

    int N = atoi(argv[2]);

    engine_t eng;
    std::thread th([=, &eng]{
        eng.run();
    });

    address_t addr((const char*)"127.0.0.1", 8001);
    std::shared_ptr<dialer_t<rpc_request_t, rpc_response_t>> cli = eng.dial<rpc_request_t, rpc_response_t>(&addr);
    fprintf(stderr, "client [%s] boot up.\n", "127.0.0.1:8001");

    fprintf(stderr, "N:%d\n", N);
    for(int i=0; i<N; ++i){
        rpc_request_t *req = new rpc_request_t;
        const char *s = "test, 123!";
        req->setbody(s, strlen(s));

        fprintf(stderr, "cli call\n");
        cli->call(req, rpcdone);
    }

    th.join();
    return 0;
}
