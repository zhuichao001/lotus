#include <stdio.h>
#include "lotus.h"


int main(int argc, char *argv[]) {
    auto *svr = new rpc_service_t([](rpc_session_t *ses) ->int {
        fprintf(stderr, "rpc server process req=%s\n", ses->request()->data()); 
        //sleep(1); // simulate calculate duration
        rpc_response_t rsp(ses->request()->data(), ses->request()->len());
        rsp.setmsgid(ses->request()->msgid());
        ses->reply(&rsp);
        return 0;
    });

    engine_t eng;
    address_t addr((const char*)"127.0.0.1", 8001);
    eng.start(&addr, svr);
    fprintf(stderr, "server [%s] boot up.\n", "127.0.0.1:8001");
    eng.run();
    return 0;
}
