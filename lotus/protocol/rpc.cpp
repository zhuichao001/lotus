#include "rpc.h"

int rpc_message_t::base_msgid = 1000000;

int rpc_message_t::encode(buff_t *to)const{
    char *dest = to->data();
    dest[0] = _msgtype;

    uint64_t tmp_msgid = _msgid;
    for(int i=0; i<sizeof(uint64_t); ++i){
        dest[1+i] = tmp_msgid & 0xff;
        tmp_msgid >>= sizeof(uint64_t);
    }

    uint32_t tmp_bodylen = _bodylen;
    for(int i=0; i<sizeof(uint32_t); ++i){
        dest[9+i] = tmp_bodylen & 0xff;
        tmp_bodylen >>= 8;
    }

    memcpy(dest+13, data(), len());
    const int msglen = 13+len();
    to->expend(msglen);
    return msglen;
}

int rpc_message_t::decode(buff_t *from){
    if(from->len()<13){
        //fprintf(stderr, "warning: buff len:%d is too less\n", from->len());
        return 0;
    }

    char *data = from->data();
    _msgtype = uint8_t(data[0]);

    _msgid = 0;
    for(int i=0; i<sizeof(uint64_t); ++i){
        _msgid = (_msgid<<8) + uint8_t(data[8-i]);
    }

    _bodylen = 0;
    for(int i=0; i<sizeof(uint32_t); ++i){
        _bodylen = (_bodylen<<8) + uint8_t(data[12-i]);
    }

    _body->reset();
    _body->append(data+13, _bodylen);
    return 13+_bodylen;
}
