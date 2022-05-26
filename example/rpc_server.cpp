#include <stdio.h>
#include "engine.h"
#include "service.h"
#include "session.h"
#include "protocol/rpc.h"


int process(session_t<rpc_request_t, rpc_response_t> *ses){
    fprintf(stderr, "rpc server process req=%s\n", ses->request()->data()); 
    sleep(1); // simulate calculate duration
    rpc_response_t rsp(ses->request()->data(), ses->request()->len());
    ses->reply(&rsp);
    return 0;
}

int main(int argc, char *argv[]) {
    engine_t eng;
    address_t addr((const char*)"127.0.0.1", 8001);

    auto *svr = new service_t<rpc_request_t, rpc_response_t>();
    svr->process = process;
    eng.start(&addr, svr);
    fprintf(stderr, "server [%s] boot up.\n", "127.0.0.1:8001");
    eng.run();
    return 0;
}
