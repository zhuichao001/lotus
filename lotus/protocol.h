#ifndef _NET_PROTOCOL_H_
#define _NET_PROTOCOL_H_

#include <functional>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "buff.h"
#include "util.h"
#include "endpoint.h"

enum MESSAGE_TYPE{
    TYPE_REQUEST=1,
    TYPE_RESPONSE=2,
    TYPE_HEARTBEAT=3
};

class rpc_message_t{
public:
    rpc_message_t(MESSAGE_TYPE type, int size):
        _msgtype(type),
        _bodylen(0){
        _body = new buff_t(size);
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

    uint64_t msgid()const{
        return _msgid;
    }

    void setmsgid(uint64_t id){
        _msgid = id;
    }

    int write(const char *data, int len){
        _body->append(data, len);
        _bodylen += len;
        return len;
    }

    void setbody(const char* body, int len){
        write(body, len);
    }

    static int base_msgid;

private:
    uint8_t _msgtype;
    uint64_t _msgid;
    int32_t _bodylen;
    buff_t *_body;
};

class rpc_request_t : public rpc_message_t{
public:
    rpc_request_t():
        rpc_message_t(TYPE_REQUEST, 128){
        setmsgid(++rpc_message_t::base_msgid);
    }
};

enum ErrorCode{
    RPC_OK = 0,
    RPC_ERR_TIMEOUT = -101,
    RPC_ERR_CONNCLOSE = -102
};

class rpc_response_t : public rpc_message_t{
public:
    rpc_response_t():
        rpc_message_t(TYPE_RESPONSE, 128),
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

typedef std::function<int(rpc_request_t*, rpc_response_t *)> RpcCallback;

#endif
