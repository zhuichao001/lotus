#include <stdio.h>
#include "carriage.h"
#include "server.h"

int main(int argc, char *argv[]) {
    carriage_t carry;
    address_t addr((const char*)"127.0.0.1", 8001);
    carry.start(&addr);
    fprintf(stderr, "client [%s] boot up.\n", "127.0.0.1:8001");
    carry.run();
    return 0;
}
