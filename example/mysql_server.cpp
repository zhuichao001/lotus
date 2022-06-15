#include <stdio.h>
#include "lotus.h"


int main(int argc, char *argv[]) {
    auto *svr = new mysql_service_t([](mysql_session_t *ses) ->int {
        mysql_response_t rsp;
        rsp.print();

        ses->reply(&rsp);
        return 0;
    });

    engine_t eng;
    address_t addr((const char*)"0.0.0.0", 3306);

    eng.start(&addr, svr);
    fprintf(stderr, "server [%s] boot up.\n", "0.0.0.0:3306");
    eng.run();
    return 0;
}
