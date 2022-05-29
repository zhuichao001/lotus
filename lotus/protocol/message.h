#pragma once

enum MESSAGE_TYPE{
    TYPE_REQUEST=1,
    TYPE_RESPONSE=2,
    TYPE_HEARTBEAT=3
};

class message_t{
public:
    uint8_t _msgtype;
    uint64_t _msgid;
    message_t(MESSAGE_TYPE type):
        _msgtype(type){
    }
    uint64_t msgid()const{
        return _msgid;
    }

    void setmsgid(uint64_t id){
        _msgid = id;
    }
};
