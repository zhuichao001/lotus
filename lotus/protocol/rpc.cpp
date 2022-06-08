#include "rpc.h"

uint64_t rpc_request_t::BASE_MSGID = 10000000;

const int MSG_HEAD_LEN = 17;

int rpc_message_t::encode(std::string &dat)const{
    buff_t to(2048);
    encode(&to);
    dat.clear();
    dat.assign(to.data(), to.len());
    return to.len();
}

int rpc_message_t::encode(buff_t *to)const{
    char *dest = to->data();
    dest[0] = _msgtype;

    uint64_t tmp_msgid = _msgid;
    for(int i=0; i<sizeof(uint64_t); ++i){
        dest[1+i] = tmp_msgid & 0xff;
        tmp_msgid >>= sizeof(uint64_t);
    }

    int32_t tmp_errcode = _errcode;
    for(int i=0; i<sizeof(int32_t); ++i){
        dest[9+i] = tmp_errcode & 0xff;
        tmp_errcode >>= 8;
    }

    uint32_t tmp_bodylen = _body->len();
    for(int i=0; i<sizeof(uint32_t); ++i){
        dest[13+i] = tmp_bodylen & 0xff;
        tmp_bodylen >>= 8;
    }
    to->expend(MSG_HEAD_LEN); //update length after directly write

    to->append(_body->data(), _body->len());

    const int msglen = MSG_HEAD_LEN + _body->len();
    return msglen;
}

int rpc_message_t::decode(buff_t *from){
    return decode(from->data(), from->len());
}

int rpc_message_t::decode(const char *data, int len){
    if(len<MSG_HEAD_LEN){
        return 0;
    }

    _msgtype = uint8_t(data[0]);

    _msgid = 0;
    for(int i=0; i<sizeof(uint64_t); ++i){
        _msgid = (_msgid<<8) + uint8_t(data[8-i]);
    }

    _errcode = 0;
    for(int i=0; i<sizeof(int32_t); ++i){
        _errcode = (_errcode<<8) + uint8_t(data[12-i]);
    }

    uint32_t bodylen = 0;
    for(int i=0; i<sizeof(uint32_t); ++i){
        bodylen = (bodylen<<8) + uint8_t(data[16-i]);
    }

    if(bodylen+MSG_HEAD_LEN < len){
        fprintf(stderr, "message body not complete\n");
        return 0;
    }

    _body->reset();
    _body->append(data + MSG_HEAD_LEN, bodylen);

    const int msglen = MSG_HEAD_LEN + bodylen;
    return msglen;
}
