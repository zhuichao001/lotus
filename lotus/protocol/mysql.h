#pragma once

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include "../buff.h"


class mysql_request_t {
public:
    mysql_request_t(){
    }

    mysql_request_t() {
    }

    int encode(buff_t *to);

    int decode(buff_t *from);

    void print(){
        buff_t buf(512);     
        this->encode(&buf);
        fprintf(stderr, "print mysql_request_t------\n%s\n", buf.data());
    }
};

class mysql_response_t {
public:
    mysql_response_t(){
    }

    int encode(buff_t *to);

    int decode(buff_t *from);

    void print(){
        buff_t buf(512);     
        encode(&buf);
        fprintf(stderr, "print mysql_response_t------\n%s\n", buf.data());
    }
};
