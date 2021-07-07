#ifndef _NET_PROTOCOL_H_
#define _NET_PROTOCOL_H_

#include "buff.h"
#include "util.h"

enum MESSAGE_TYPE{
    TYPE_REQUEST=0,
    TYPE_RESPONSE,
    TYPE_HEARTBEAT
};

class message_t{
public:
    message_t(MESSAGE_TYPE type, int size):
        _msgtype(type){
        _msgid = get_nanosec(); //FIXME
        _body = new buff_t(size);
    }
    
    ~message_t(){
        delete _body;
    }

    int encode(buff_t *b){
        const char *s = "hello";
        _body->append(s, 6);
        return 0;
    }

    char *data(){
        return _body->data();
    }

    int len(){
        return _body->len();
    }

    int decode(buff_t *b){
        return 0;
    }

    int write(const char *data, int len){
        _body->append(data, len);
    }

    long long _msgid;
    int _msgtype;
    buff_t *_body;
};

class request_t{
public:
    request_t():
        msg(TYPE_REQUEST, 2048){
    }

    int encode(buff_t *b){
        return msg.encode(b);
    }

    int decode(buff_t *b){
        return msg.decode(b);
    }

    char * data(){
        return msg.data();
    }

    int len(){
        return msg.len();
    }

    int msgid(){
        return msg._msgid;
    }

    message_t msg;
};

class response_t{
public:
    response_t():
        msg(TYPE_RESPONSE, 2048),
        errcode(0){
    }

    int encode(buff_t *b){
        return msg.encode(b);
    }

    int decode(buff_t *b){
        return msg.decode(b);
    }

    int write(const char *data, int len){
        msg.write(data, len);
    }

    int msgid(){
        return msg._msgid;
    }


    message_t msg;
    int errcode;
};

#endif
