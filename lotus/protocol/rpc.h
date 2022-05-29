#pragma once

#include <functional>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "message.h"
#include "../buff.h"
#include "../util.h"

class rpc_message_t:
    public message_t {
public:
    rpc_message_t(MESSAGE_TYPE msgtype, int maxsize):
        message_t(msgtype),
        _bodylen(0){
        _body = new buff_t(maxsize);
    }
    
    ~rpc_message_t(){
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

    int write(const char *data, int len){
        _body->append(data, len);
        _bodylen += len;
        return len;
    }

    void setbody(const char* body, int len){
        write(body, len);
    }

private:
    int32_t _bodylen;
    buff_t *_body;
};

class rpc_request_t : public rpc_message_t{
public:
    rpc_request_t():
        rpc_message_t(TYPE_REQUEST, 256){
        setmsgid(++rpc_request_t::BASE_MSGID);
    }

    static uint64_t BASE_MSGID;
};

enum ErrorCode{
    RPC_OK = 0,
    RPC_ERR_TIMEOUT = -101,
    RPC_ERR_CONNCLOSE = -102
};

class rpc_response_t : public rpc_message_t{
public:
    rpc_response_t():
        rpc_message_t(TYPE_RESPONSE, 256),
        _errcode(0){
    }

    rpc_response_t(const char* body, int len, int err=RPC_OK):
        rpc_message_t(TYPE_RESPONSE, 128){
        _errcode = err;
        setbody(body, len);
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
