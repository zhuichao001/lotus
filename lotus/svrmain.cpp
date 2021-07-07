#include <stdio.h>
#include "engine.h"
#include "server.h"

int main(int argc, char *argv[]) {
    engine_t eng;
    address_t addr((const char*)"127.0.0.1", 8001);
    eng.start(&addr, new rpc_server_t());
    fprintf(stderr, "server [%s] boot up.\n", "127.0.0.1:8001");
    eng.run();
    return 0;
}
