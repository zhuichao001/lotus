#include <stdio.h>
#include "engine.h"
#include "service.h"

class rpc_service_t: public service_t{
public:
    int process(request_t *req, response_t *rsp){
        fprintf(stderr, "rpc server process req=%s\n", req->data()); 
        sleep(3); // simulate caculate duration
        rsp->setbody(req->data(), req->len());
        return 0;
    }
};

int main(int argc, char *argv[]) {
    engine_t eng;
    address_t addr((const char*)"127.0.0.1", 8001);
    eng.start(&addr, new rpc_service_t());
    fprintf(stderr, "server [%s] boot up.\n", "127.0.0.1:8001");
    eng.run();
    return 0;
}
