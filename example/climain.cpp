﻿#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include "protocol.h"
#include "engine.h"
#include "dialer.h"

int rpcdone(response_t *rsp){
    if(rsp->errcode()!=0){
        fprintf(stderr, "rpc failed, response error code:[%d]\n", rsp->errcode());
        return -1;
    }
    fprintf(stderr, "call rpc done, response:[%s]\n", rsp->data());
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc<2){
        fprintf(stderr, "usage: -n [rpc count]\n");
        return 0;
    }

    int N = atoi(argv[1]);

    engine_t eng;
    std::thread th([=, &eng]{
        eng.run();
    });

    address_t addr((const char*)"127.0.0.1", 8001);
    dialer_t *cli = eng.open(&addr);
    fprintf(stderr, "client [%s] boot up.\n", "127.0.0.1:8001");

    for(int i=0; i<N; ++i){ //100w
        request_t req;
        const char *s = "test, 123!";
        req.setbody(s, strlen(s));

        cli->call(&req, rpcdone);
    }

    th.join();
    return 0;
}