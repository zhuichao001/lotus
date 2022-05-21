#include <stdio.h>
#include "engine.h"
#include "service.h"
#include "session.h"

class rpc_service_t: public service_t{
public:
    int process(session_t *ses){
        fprintf(stderr, "rpc server process req=%s\n", ses->request()->data()); 
        sleep(10); // simulate caculate duration
        response_t rsp(ses->request()->data(), ses->request()->len());
        ses->reply(&rsp);
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
