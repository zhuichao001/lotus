#include <stdio.h>
#include <thread>
#include "protocol.h"
#include "dialer.h"
#include "server.h"

int rpcdone(request_t *req, response_t *res){
    fprintf(stderr, "call done\n");
    return -1;
}

int main(int argc, char *argv[]) {
    dialer_t dialer;
    std::thread th([=, &dialer]{
        dialer.run();
    });

    address_t addr((const char*)"127.0.0.1", 8001);
    rpc_client_t *cli = dialer.caller(&addr);

    request_t req;
    cli->call(&req, rpcdone);
    fprintf(stderr, "client [%s] boot up.\n", "127.0.0.1:8001");

    th.join();
    return 0;
}
