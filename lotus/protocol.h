#ifndef _NET_PROTOCOL_H_
#define _NET_PROTOCOL_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "buff.h"
#include "util.h"

enum MESSAGE_TYPE{
    TYPE_REQUEST=0,
    TYPE_RESPONSE,
    TYPE_HEARTBEAT
};

static int base_msgid=0;

class message_t{
public:
    message_t(MESSAGE_TYPE type, int size):
        _msgtype(type),
        _bodylen(0){
        _msgid = ++base_msgid; //FIXME
        _body = new buff_t(size);
    }
    
    ~message_t(){
        delete _body;
    }

    int encode(buff_t *to){
        char *dest = to->data();
        dest[0] = _msgtype;

        uint64_t tmp_msgid = _msgid;
        for(int i=0; i<8; ++i){
            dest[1+i] = tmp_msgid & 0xff;
            tmp_msgid >>= 8;
        }

        uint32_t tmp_bodylen = _bodylen;
        for(int i=0; i<4; ++i){
            dest[9+i] = tmp_bodylen & 0xff;
            tmp_bodylen >>= 8;
        }

        memcpy(dest+13, data(), len());
        const int msglen = 13+len();
        to->expend(msglen);
        return msglen;
    }

    char *data(){
        return _body->data();
    }

    int len(){
        return _body->len();
    }

    uint64_t msgid(){
        return _msgid;
    }

    int decode(buff_t *from){
        if(from->len()<13){
            fprintf(stderr, "warning: buff len:%d is too less\n", from->len());
            return 0;
        }

        char *data = from->data();
        _msgtype = uint8_t(data[0]);
        fprintf(stderr, "_msgtype:%d\n", _msgtype);

        _msgid = 0;
        for(int i=0; i<8; ++i){
            _msgid = (_msgid<<8) + int(data[8-i]);
        }
        fprintf(stderr, "_msgid:%ld\n", _msgid);

        _bodylen = 0;
        for(int i=0; i<4; ++i){
            _bodylen = (_bodylen<<8) + int(data[12-i]);
        }
        fprintf(stderr, "_bodylen:%d\n", _bodylen);

        _body->reset();
        _body->append(data+13, _bodylen);
        return 13+_bodylen;
    }

    int write(const char *data, int len){
        _body->append(data, len);
        _bodylen += len;
    }

private:
    uint8_t _msgtype;
    uint64_t _msgid;
    int32_t _bodylen;
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

    int setbody(const char* body, int len){
        msg.write(body, len);
    }

    const char * data(){
        return msg.data();
    }

    int len(){
        return msg.len();
    }

    int msgid(){
        return msg.msgid();
    }

private:
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

    int setbody(const char *data, int len){
        return msg.write(data, len);
    }

    void seterrcode(int err){
        errcode = err;
    }

    const char * data(){
        return msg.data();
    }

    int len(){
        return msg.len();
    }

    int msgid(){
        return msg.msgid();
    }

private:
    message_t msg;
    int errcode;
};

#endif
