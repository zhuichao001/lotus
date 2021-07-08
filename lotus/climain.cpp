#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include "protocol.h"
#include "dialer.h"
#include "server.h"

int rpcdone(request_t *req, response_t *rsp){
    fprintf(stderr, "call rpc done, response:[%s]\n", rsp->data());
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc<2){
        fprintf(stderr, "usage: -n [rpc count]\n");
        return 0;
    }

    int N = atoi(argv[1]);

    dialer_t dialer;
    std::thread th([=, &dialer]{
        dialer.run();
    });

    address_t addr((const char*)"127.0.0.1", 8001);
    rpc_client_t *cli = dialer.open(&addr);
    fprintf(stderr, "client [%s] boot up.\n", "127.0.0.1:8001");

    for(int i=0; i<N; ++i){ //100w
        request_t req;
        const char *s = "test, 123, abc, @_@";
        req.setbody(s, strlen(s)+1);
        cli->call(&req, rpcdone);
    }

    th.join();
    return 0;
}
