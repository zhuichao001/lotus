#include <stdio.h>
#include "lotus.h"


int main(int argc, char *argv[]) {
    auto *svr = new http_service_t([](http_session_t *ses) ->int {
        fprintf(stderr, "http server process \n"); 
        sleep(1); // simulate calculate duration
        http_response_t rsp;
        rsp.set_response(OK, "hello,world!");
        ses->reply(&rsp);
        return 0;
    });

    engine_t eng;
    address_t addr((const char*)"0.0.0.0", 8080);

    eng.start(&addr, svr);
    fprintf(stderr, "server [%s] boot up.\n", "0.0.0.0:8080");
    eng.run();
    return 0;
}
