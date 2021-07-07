﻿#include <stdio.h>
#include <thread>
#include "protocol.h"
#include "dialer.h"
#include "server.h"

int rpcdone(request_t *req, response_t *rsp){
    fprintf(stderr, "call done, response data:%s\n", rsp->data());
    return -1;
}

int main(int argc, char *argv[]) {
    dialer_t dialer;
    std::thread th([=, &dialer]{
        dialer.run();
    });

    address_t addr((const char*)"127.0.0.1", 8001);
    rpc_client_t *cli = dialer.newclient(&addr);
    fprintf(stderr, "client [%s] boot up.\n", "127.0.0.1:8001");

    request_t req;
    const char *s = "hello, world";
    req.setbody(s, strlen(s)+1);
    cli->call(&req, rpcdone);

    th.join();
    return 0;
}
