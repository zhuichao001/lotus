#ifndef _NET_PROTOCOL_H_
#define _NET_PROTOCOL_H_

#include <functional>
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


class message_t{
public:
    message_t(MESSAGE_TYPE type, int size):
        _msgtype(type),
        _bodylen(0){
        _body = new buff_t(size);
    }
    
    ~message_t(){
        delete _body;
    }

    int encode(buff_t *to)const;

    int decode(buff_t *from);

    const char *data()const{
        return _body->data();
    }

    int len()const{
        return _body->len();
    }

    uint64_t msgid()const{
        return _msgid;
    }

    void setmsgid(uint64_t id){
        _msgid = id;
    }

    int write(const char *data, int len){
        _body->append(data, len);
        _bodylen += len;
    }

    int setbody(const char* body, int len){
        write(body, len);
    }

    static int base_msgid;

private:
    uint8_t _msgtype;
    uint64_t _msgid;
    int32_t _bodylen;
    buff_t *_body;
};

class request_t : public message_t{
public:
    request_t():
        message_t(TYPE_REQUEST, 128){
        setmsgid(++message_t::base_msgid);
    }
};

enum ErrorCode{
    OK = 0,
    RPC_TIMEOUT = -101
};

class response_t : public message_t{
public:
    response_t():
        message_t(TYPE_RESPONSE, 128),
        _errcode(0){
    }

    void seterrcode(int32_t err){
        _errcode = err;
    }

    int32_t errcode(){
        return _errcode;
    }

private:
    int32_t _errcode;
};

typedef std::function<int(response_t *)> RpcCallback;

#endif
